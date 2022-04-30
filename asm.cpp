#include <iostream>
#include <stdio.h>
#include <sstream>
#include <bitset>
#include <cstdint>

using namespace std;

// all commands
int condition(uint32_t cmd) { return cmd >> 28; }
int opcode(uint32_t cmd) { return (cmd >> 26) & 0b0011; } 

int getRd(uint32_t cmd) { 
    int shifted = cmd >> 12;
    shifted = shifted & 0b1111;
    return shifted;
    // return (cmd >> 12) & 0b1111; 
}
int getRn(uint32_t cmd) { 
    int shifted = cmd >> 16;
    shifted = shifted & 0b1111;
    return shifted;
}
int getRm(uint32_t cmd) { 
    return cmd & 0b1111;
}

int getRs(uint32_t cmd) { 
    return (cmd >> 8) & 0b1111;
}
bool getBit4(uint32_t cmd) {
    return (cmd >> 4) & 0b01;
}
int getShAmt(uint32_t cmd) {
    return (cmd >> 7) & 0b11111;
}
int getShType(uint32_t cmd) {
    return (cmd >> 5) & 0b11;
}

// arithmetic
bool arithImm(uint32_t cmd) { return (cmd >> 25) & 0b0001; }
int getImm(uint32_t cmd) { return cmd & 0b11111111; }
string arithCmd(uint32_t cmd) { 
    cmd = (cmd >> 21) & 0b1111; 
    switch(cmd) {
        case 0b0100: { return "add"; }
        case 0b0000: { return "and"; }
        case 0b1110: { return "bic"; }
        case 0b1011: { return "cmn"; }
        case 0b1010: { return "cmp"; }
        case 0b0001: { return "eor"; }
        case 0b1101: { return "mov"; } 
        case 0b1111: { return "mvn"; }
        case 0b1100: { return "orr"; }
        case 0b0010: { return "sub"; }
        default: { return ""; }
    }
}
string getShCommand(int shtype) {
    switch (shtype) {
        case 0b00: { return "lsl"; }
        case 0b01: { return "lsr"; }
        case 0b10: { return "asr"; }
        case 0b11: { return "ror"; }
        default: { return ""; }
    }
}

// load-store
int getIPUBWL(uint32_t cmd) {
    return (cmd >> 20) & 0b111111;
}
string getLSCommand(int ipubwl) {
    string command = ipubwl & 0b01 == 0 ? "str" : "ldr"; 
    command += (ipubwl >> 1) & 0b01 == 1 ? "b":""; 
    return command;
}
int getLSImm(uint32_t cmd) {
    return cmd & 0b111111111111;
}

// branch
int getLink(uint32_t cmd) {
    return (cmd >> 25) & 0b01;
}
int getBImm(uint32_t cmd) {
    return (cmd >> 24) & 0b111111111111111111111111;
}

int main() {
    printf("Enter the ARMv4 Command as an 8-character hexadecimal string\nEnter 0 to exit\n");
    string command;
    cin >> command;

    while (command != "0") {
        uint32_t val;
        istringstream(command) >> std::hex >> val;
        string output = "";
        switch (opcode(val)) {

            // arithmetic opcode 00
            case 0: {
                bool immediate = arithImm(val);
                string cmd = "";
                cmd += arithCmd(val) + " ";

                if(arithCmd(val) == "cmp") {
                    cmd += "r" + to_string(getRn(val)) + ", ";
                    if(immediate) {
                        cmd += "#" + to_string(getImm(val));
                    } else {
                        cmd += "r" + to_string(getRm(val));
                    }
                    output = cmd;
                    break;
                }

                cmd += "r" + to_string(getRd(val)) + ", ";
                if(arithCmd(val) != "mov") {
                    cmd += "r" + to_string(getRn(val)) + ", ";
                }
                if(immediate) {
                    // immediate data
                    cmd += "#" + to_string(getImm(val));
                } else if(getBit4(val) == 0) {
                    // register data
                    cmd += "r" + to_string(getRm(val));
                    if(getShAmt(val) != 0) {
                        cmd += ", " + getShCommand(getShType(val)) + " ";
                        cmd += "#" + to_string(getShAmt(val));
                    }

                } else {
                    // register data
                    cmd += "r" + to_string(getRm(val));

                    if((val >> 7) & 1 == 0) {
                        cmd += ", " + getShCommand(getShType(val)) + " ";
                        cmd += "r" + to_string(getRs(val));
                    }
                }
                output = cmd;
                break;
            }                 
            // load-store opcode 01
            case 1: {
                int ipubwl = getIPUBWL(val);
                bool immediate = (ipubwl >> 5);
                string cmd = "";

                cmd += getLSCommand(ipubwl) + " "; 
                cmd += "r" + to_string(getRd(val)) + ", [" + to_string(getRn(val));
                if(immediate) {
                    cmd += ", #" + to_string(getLSImm(val));
                }
                cmd += "]";
                output = cmd;
                break;
            }

            // branch opcode     10
            case 2: {   
                string cmd = "";
                if(condition(val) == 14) {
                    // branch always or branch link
                    if(getLink(val) == 1) {
                        cmd += "bl ";
                    } else {
                        cmd += "bal ";
                    }
                } else if(condition) {
                    // branch equals
                    cmd += "beq ";
                } else {
                    cmd += "bne ";
                    // branch not equals
                }
                cmd += "0b" + to_string(getBImm(val));
                output = cmd;
                break;
            }
            default:
                break;
        }
        cout << output;

        printf("\nEnter the ARMv4 Command as an 8-character hexadecimal string\nEnter 0 to exit\n");
        cin >> command;

    }
    printf("stay noided");
}