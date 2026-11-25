#include "DSP.h"
#include "controller.h"
#include "dataProcessing.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <chrono>
#include <utility>


int blocksize = 379; 
int samplerate = 8000; 
int channels = 1; 
int computerIDudp = 4;
int computerIDtcp = 5;

DSP dsp(blocksize, samplerate, channels, {computerIDudp, computerIDtcp});
DataProcessing dP;
std::vector<int> toner = {};
std::vector<bool> executed = {true, true};

std::deque<std::vector<int>> queueList;

std::mutex mtx;

std::vector<int> dataPlayer1 = {0, 0, 0, 0};
std::vector<int> dataPlayer2 = {0, 0, 0, 0};

void player1()
{
    int playerNumber = 0;
    int robotIDudp = 0;
    int robotIDtcp = 1;
    std::vector<int> data = {0, 0, 0, 0}; 
    int lastPressedKey = -1;

    Controller input;    
    while(1)
    {
        int pakkenr = 0;
        switch (input.readKey()) {
            case Key::Up:
                mtx.lock();
                std::cout << "Up arrow pressed" << std::endl;
                if (lastPressedKey != 1){
                    lastPressedKey = 1;
                    dataPlayer1 = {robotIDudp, 7, 7, 7};
                    executed[playerNumber] = false;
                }
                mtx.unlock();
                break;
            case Key::Down:
                mtx.lock();
                std::cout << "Down arrow pressed" << std::endl;
                if (lastPressedKey != 2){
                    lastPressedKey = 2;
                    std::vector<std::int32_t> dataBeforeCRC = {robotIDtcp, pakkenr/16, pakkenr%16};
                    std::uint8_t crc = dP.crc8(dataBeforeCRC, dataBeforeCRC.size());
                    std::cout << "crc: " << crc <<std::endl;
                    dataPlayer1 = {robotIDtcp, pakkenr/16, pakkenr%16, 4, crc/16, crc%16};
                    std::cout << "player1Data: " << dataPlayer1[0] << ", " << dataPlayer1[1] << ", " << dataPlayer1[2] << ", " << dataPlayer1[3] << std::endl;
                    executed[playerNumber] = false;
                }
                pakkenr++;
                mtx.unlock();
                break;
            case Key::Left:
                std::cout << "Left arrow pressed" << std::endl;
                if (lastPressedKey != 3){
                    lastPressedKey = 3;
                    dataPlayer1 = {robotIDudp, 15, 15, 15};
                    executed[playerNumber] = false;
                }
                break;
            case Key::Right:
                mtx.lock();
                std::cout << "Right arrow pressed" << std::endl;
                if (lastPressedKey != 4){
                    lastPressedKey = 4;
                    dataPlayer1 = {robotIDudp, 0, 0, 0};
                    executed[playerNumber] = false;
                }
                mtx.unlock();
                break;
            default:
                mtx.lock();
                std::cout << "Other key pressed" << std::endl;
                mtx.unlock();
                break;
            }
        
    }

}

void player2()
{
    int playerNumber = 1;
    int robotIDudp = 2;
    int robotIDtcp = 3;
    std::vector<int> data = {0, 0, 0, 0};   
    int lastPressedKey = -1;

    Controller input;    
    while(1)
    {
        
        int pakkenr = 0;
        switch (input.readKey()) {
            case Key::W:
                mtx.lock();
                if (lastPressedKey != 1){
                    lastPressedKey = 1;
                    dataPlayer2 = {robotIDudp, 7, 7, 7};
                    executed[playerNumber] = false;
                }
                mtx.unlock();
                break;
            case Key::S:
                mtx.lock();
                if (lastPressedKey != 2){
                    lastPressedKey = 2;
                    dataPlayer2 = {robotIDtcp, pakkenr/16, pakkenr%16, 4, 11, 15};
                    executed[playerNumber] = false;
                }
                pakkenr++;
                mtx.unlock();
                break;
            case Key::A:
                mtx.lock();
                if (lastPressedKey != 3){
                    lastPressedKey = 3;
                    dataPlayer2 = {robotIDudp, 15, 15, 15};
                    executed[playerNumber] = false;
                }
                mtx.unlock();
                break;
            case Key::D:
                mtx.lock();
                if (lastPressedKey != 4){
                    lastPressedKey = 4;
                    dataPlayer2 = {robotIDudp, 0, 0, 0};
                    executed[playerNumber] = false;
                }
                mtx.unlock();
                break;
            default:
                mtx.lock();
                std::cout << "Other key pressed" << std::endl;
                mtx.unlock();
                break;
            }
        
    }

}



void playerInput()
{
    const int player1Udp = 0;
    const int player1Tcp = 1;
    const int player2Udp = 2;
    const int player2Tcp = 3;

    std::vector<int> lastPressedKey = {-1, -1};
    Controller input;

    int pakkenrPlayer1 = 0;
    int pakkenrPlayer2 = 0;

    while (true) {
        const Key key = input.readKey();
        int player = -1;
        int pressedKey = -1;
        std::vector<int> data;

        switch (key) {
            case Key::Up:    player = 0; pressedKey = 1; data = {player1Udp, 7, 7, 7}; break;

            case Key::Down:
                player = 0; pressedKey = 2;
                {
                    std::vector<std::int32_t> dataBeforeCRC = {
                        player1Tcp, pakkenrPlayer1/16, pakkenrPlayer1%16, 4
                    };
                    std::uint8_t crc = dP.crc8(dataBeforeCRC, dataBeforeCRC.size());
                    std::cout << "crc: " << (int)crc << std::endl;

                    data = {player1Tcp, pakkenrPlayer1/16, pakkenrPlayer1%16, 4, crc/16, crc%16};
                    std::cout << "data: " << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ", " << data[4] << ", " << data[5] << std::endl;
                    pakkenrPlayer1++;
                }
                break;

            case Key::Left:  player = 0; pressedKey = 3; data = {player1Udp, 15, 15, 15}; break;
            case Key::Right: player = 0; pressedKey = 4; data = {player1Udp, 0, 0, 0}; break;

            case Key::W:     player = 1; pressedKey = 1; data = {player2Udp, 7, 7, 7}; break;

            case Key::S:
                player = 1; pressedKey = 2;
                {
                    std::vector<std::int32_t> dataBeforeCRC = {
                        player2Tcp, pakkenrPlayer2/16, pakkenrPlayer2%16, 4
                    };
                    std::uint8_t crc = dP.crc8(dataBeforeCRC, dataBeforeCRC.size());
                    std::cout << "crc: " << (int)crc << std::endl;

                    data = {player2Tcp, pakkenrPlayer2/16, pakkenrPlayer2%16, 4, crc/16, crc%16};
                    pakkenrPlayer2++;
                }
                break;

            case Key::A:     player = 1; pressedKey = 3; data = {player2Udp, 15, 15, 15}; break;
            case Key::D:     player = 1; pressedKey = 4; data = {player2Udp, 0, 0, 0}; break;

            case Key::Other:
                std::cout << "Other key pressed\n";
                continue;
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            if (lastPressedKey[player] != pressedKey) {
                lastPressedKey[player] = pressedKey;
                (player == 0 ? dataPlayer1 : dataPlayer2) = std::move(data);
                executed[player] = false;
            }
        }

        std::cout << (player == 0 ? "Player 1 key pressed\n" : "Player 2 key pressed\n");
    }
}


void queue()
{
    int nextPlayerCheck = 0;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            const std::vector<int>& data = nextPlayerCheck == 0 ? dataPlayer1 : dataPlayer2;
            if (!executed[nextPlayerCheck]) {
                bool dataExists = false;
                for (auto& queuedData : queueList) {
                    if (!queuedData.empty() && data[0] == queuedData[0]) {
                        queuedData = data;
                        dataExists = true;
                        break;
                    }
                }
                if (!dataExists) {
                    queueList.push_back(data);
                }
                executed[nextPlayerCheck] = true;
            }
            nextPlayerCheck = 1 - nextPlayerCheck;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void playTones()
{   
    std::vector<int> dataCopy;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!queueList.empty()) {
                dataCopy = std::move(queueList.front());
                queueList.pop_front();
            }
        }
        if (dataCopy.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        dsp.sendData(dataCopy);
        dataCopy.clear();
    }
}

int main()
{
    std::thread t1(playerInput);
    std::thread t3(queue);
    std::thread t4(playTones);
    bool run = true;
    while (run) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (dataPlayer1[3] == 7) {
                run = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    t1.join();
    t3.join();
    t4.join();

    return 0;
}