#include <mutex>

struct dataPacket {};
struct connectionInfo {};
struct connectionHandle {
    void sendData(dataPacket);
    dataPacket receiveData();
};
struct connectionManager {
    connectionHandle open(connectionInfo);
};

connectionManager someConnectionManager;

class X {
    private:
        connectionInfo connectionDetails;
        connectionHandle connection;
        std::once_flag connectionInitFlag;
        void openConnection() {
            connection = someConnectionManager.open(connectionDetails);
        }
    public:
        X(connectionInfo const & connectionDetails_) : connectionDetails(connectionDetails_) {}
        void sendData(dataPacket const & data) {
            std::call_once(connectionInitFlag, &X::openConnection, this);
            connection.sendData(data);
        }
        dataPacket receiveData() {
            std::call_once(connectionInitFlag, &X::openConnection, this);
            return connection.receiveData();
        }
};

// Guaranteed safe initialization for single global instance:
class myClass {};
myClass & getMyClassInstance() {
    static myClass instance;
    return instance;
}