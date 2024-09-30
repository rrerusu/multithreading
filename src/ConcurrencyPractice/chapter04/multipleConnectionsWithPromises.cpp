#include <future>

// struct ConnectionSet {
//     ConnectionIterator begin();
//     ConnectionIterator end();
// };
// struct ConnectionIterator {
//     bool operator!=(const ConnectionIterator & rhs);
//     void operator++();
    
//     bool hasIncomingData();
//     bool hasOutgoingData();

// };

// bool done(ConnectionSet &);

template<typename payloadType>
void processConnections(ConnectionSet& connections) {
    while(!done(connections)) {
        for(ConnectionIterator connection = connections.begin(), end = connections.end();
            connection != end;
            ++connection) {
                if(connection.hasIncomingData()) {
                    dataPacket data = connection.incomming();
                    std::promise<payloadType> & p = connection.getPromise(data.id);
                    p.set_value(data.payload);
                }
                if(connection.hasOutgoingData()) {
                    ouutgoingPacket data = connection.topOfOutgoingQueue();
                    connection.send(data.payload);
                    data.promise.set_value(true)
                }
        }
    }
}