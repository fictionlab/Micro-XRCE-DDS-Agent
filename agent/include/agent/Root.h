// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROOT_H
#define _ROOT_H

#include <agent/ObjectVariant.h>
#include <agent/client/ProxyClient.h>
#include <agent/XRCEFactory.h>
#include <agent/XRCEParser.h>

#include <transport/ddsxrce_transport.h>

#include <thread>
#include <memory>
#include <map>

namespace eprosima{
namespace micrortps{

class Agent;

Agent& root();

/**
 * Class XRCE Agent. Handle XRCE messages and distribute them to different ProxyClients. It implements XRCEListener interface
 * for receibe messages from a XRCEParser.
 */
class Agent : public XRCEListener
{
public:
    Agent();
    ~Agent() = default;

    /*
     * Initialize the Agent.
     */
    void init();

    /*
     * Creates and stores a ProxyClient
     * @param client_key: ProxyClient unique key.
     * @param create_info: Create payload containing all the creation information.
     * @return Status struct with the operation result info.
     */
    Status create_client(int32_t client_key, const CREATE_PAYLOAD& create_info);

    /*
     * Removes a previously stored ProxyClient
     * @param client_key: ProxyClient unique key.
     * @param delete_info: Delete payload containing all the deletion information.
     * @return Status struct with the operation result info.
     */
    Status delete_client(int32_t client_key, const DELETE_PAYLOAD& delete_info);

    /*
     * Starts Agent loop to listen messages. It parses and dispaches those XRCE messages to its owner.
     */
    void run();

    /*
     * Receives a creation message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param create_payload: Creation information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_PAYLOAD& create_payload) override;

    /*
     * Receives a deletion message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param delete_payload: Deletion information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_PAYLOAD& delete_payload) override;

    /*
     * Receives a Write message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param write_payload: Write information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_PAYLOAD& write_payload)  override;

    /*
     * Receives a Read message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param read_payload: Read information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_PAYLOAD& read_payload)   override;

    ProxyClient* get_client(int32_t client_key);

private:
    channel_id_t ch_id;
    static const size_t buffer_len = 1024;
    octet out_buffer[buffer_len];
    octet in_buffer[buffer_len];
    locator_t loc;
    std::map<int32_t, ProxyClient> clients_;

    std::unique_ptr<std::thread> response_thread_;
    struct ResponseControl
    {
        std::atomic<bool> running_;
        std::atomic<bool> run_scheduled_;
        std::condition_variable condition_;
        std::mutex data_structure_mutex_;
        std::mutex condition_variable_mutex_;
    } response_control_;
    
    
    void reply();
};

} // eprosima
} // micrortps

#endif //_ROOT_H