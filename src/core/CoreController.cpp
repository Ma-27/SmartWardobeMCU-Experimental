/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 09:30
 */


#include "core/CoreController.h"
#include "core/CoreControllerBuilder.h"
#include "utility/ProjectConfig.h"
#include "data/DataChangeListener.h"

CoreController *CoreController::instance = nullptr;

CoreController *CoreController::getInstance() {
    if (instance == nullptr) {
        // 按照builder模式实例化CoreController
        CoreControllerBuilder builder;
        // 创建方法:Manager::getInstance()->检查实例并调用私有构造方法创建。
        instance = builder
                .setDataManager(DataManager::getInstance())
                .setHardwareAbstraction(HardwareAbstraction::getInstance())
                .setNetworkManager(NetworkManager::getInstance())
                .build();
    }
    return instance;
}


/** 核心控制单元中，上电初始化时首先执行的函数。
 * TODO 要严格注意其执行顺序
 */
void CoreController::init() {
    // 为DataManager订阅数据更新。
    DataChangeListener *dataChangeListener = DataChangeListener::getInstance();
    dataChangeListener->subscribeDataChange();


    // 连接网络。
    bool result = connectToWifi();

    // 返回连接结果,1表示成功，0表示失败。
    data->saveData(String(result), false);

    // 接下来的是任务调度，模拟线程为每个任务定时执行。当到了规定时间间隔后执行任务。
    // 使用Lambda表达式安排updateTemperatureAndHumidity作为任务， 更新温湿度
    scheduler.addTask([this]() { this->updateTemperatureAndHumidity(); }, ProjectConfig::UPDATE_DHT_TIME);

    scheduler.addTask([this]() { this->uploadDataToPlatform(); }, ProjectConfig::UPLOAD_DATA_TIME);
}


/**
 * 核心控制单元中，可以一直运行的函数。它负责管理整个循环
 */
void CoreController::looper() {
    // 运行任务调度器
    scheduler.run();
}

// 连接到iot服务器并且握手
bool CoreController::connectToWifi() {
    bool result = true;
    // 如果其中一个失败了，那就返回失败。

    // 提示正在连接的信息
    data->saveData("CONNECTING TO WIFI", true);
    network->setConnectionStatus(ConnectionStatus::ConnectingToWiFi);

    // 连接到Wi-Fi网络
    result = network->connectWifi() && result;

    // 提示成功连上了AP的信息
    if (result) {
        data->saveData("CONNECT SUCCEEDED TO WIFI", true);
        network->setConnectionStatus(ConnectionStatus::WiFiConnected);
    }

    // 检验服务器发回的信息正确性
    result = network->readServerShakehands() && result;

    // 提示成功连上了iot云端平台的信息
    if (result) {
        data->saveData("SERVER CONNECTED", true);
        network->setConnectionStatus(ConnectionStatus::ServerConnected);
    }

    return result;
}


// 负责更新温湿度，此过程由HardwareAbstraction层进行。
void CoreController::updateTemperatureAndHumidity() {
    // 获取到硬件抽象层实例，并且使用硬件抽象层更新温湿度，不经过此层控制
    hardware->processTemperatureAndHumidity(true);
}

// 负责上传数据到云平台 TODO 第一批数据为温湿度。
void CoreController::uploadDataToPlatform() {
    char c[100];
    // sprintf 在 Arduino 中无法转换浮点数
    dtostrf(data->temperature, 2, 2, c);
    dtostrf(data->humidity, 2, 2, c + 5);
    network->uploadDataToPlatform(String(c));
}
