#include <iostream>
#include <fstream>
#include "SHA256/src/SHA256.cpp"
#include "json/include/nlohmann/json.hpp" // using json as a database just because its easiest to set up
#include "easy-encryption/encrypt.h"

std::string RETURNKEY = "7e03f055"; // random return key, doesnt need to be hexadecimal

std::string hashString(std::string str) { // function to hash inputted string
    SHA256 sha; // initialize sha256 class
    sha.update(str); // add in string
    return SHA256::toString(sha.digest()); // return string (i still dont know what a digest is)
}

nlohmann::json readDb() { // reads the full database
    std::fstream f("../database.json"); // open json file
    nlohmann::json data = nlohmann::json::parse(f); // parse json
    f.close(); // close json file
    return data; // return parsed json
}

std::string readRawDb() { // reads the database as a string instead of json
    std::fstream f("../database.json"); // open json file
    std::string data; // make data variable
    getline(f, data); // add string of file to data variable
    f.close(); // close json file
    return data; // return json as string
}

void addRecord(std::array<std::string, 2> record) { // add a record to the database
    nlohmann::json j = readDb(); // read json file to ensure previous records are kept
    j[record[0]] = record[1]; // add record to json variable
    std::string s = j.dump(); // dump json to string
    std::fstream f("../database.json", std::ofstream::out | std::ofstream::trunc); // open json file
    f << s; // write string to json file
    f.close(); // close json file
}

void resetDb() { // resets database to original since theres no way to delete users
    std::fstream f("../database.json", std::ofstream::out | std::ofstream::trunc); // open json file
    // this function writes the default value to the database. it is an example record with the following user:
    // username: maxwell
    // passphrase: test
    f << R"({"cf407fe3eab1561eb2b4a360d21248f4b24bd33770eceb59b11d9e5469bfbfbb":"L7RrPRVmKQN="})"; // write to file
    f.close(); // close json file
}

std::string encryptPw(std::string pw) { // encrypts the returnkey string with the password as a key
    return encrypt(RETURNKEY, pw);
}

int decryptPw(std::string encrypted, std::string pw) { // checks if password can be used to decrypt string
    string guess = decrypt(encrypted, pw); // decrypts the string using the password
    return guess == RETURNKEY; // returns true if the decrypted string is equal to the returnkey
}

int main() {
    std::cout << R"(
        Choose an option of what to do:
        [1]: Log in
        [2]: Make an account
        [3]: View the database
        [4]: Reset the database
        [5]: Exit
    )";
    int query;
    std::cout << ">>> \t";
    std::cin >> query;

    if (query == 1) {
        nlohmann::json data = readDb();
        std::string username;
        std::cout << "Username: \t";
        std::cin >> username;
        username = hashString(username);
        std::string record;
        try {
            record = data[username];
        } catch (const std::exception& e) {
            std::cout << "User not found\n";
            main();
        }

        std::string passphrase;
        std::cout << "Passphrase: \t";
        std::cin >> passphrase;
        passphrase = hashString(passphrase);
        bool success = decryptPw(record, passphrase);
        if (success) {
            std::cout << "User logged in successfully\n";
            main();
        } else {
            std::cout << "Incorrect passphrase.\n";
            main();
        }
    } else if (query == 2) {
        nlohmann::json data = readDb();
        std::string username;
        std::cout << "Create a Username: \t";
        std::cin >> username;
        username = hashString(username);
        try {
            std::string record = data[username];
            cout << "There is already a user with that username.\n";
            main();
        } catch (const std::exception& e) {}

        std::string passphrase;
        std::cout << "Choose a Passphrase: \t";
        std::cin >> passphrase;
        passphrase = hashString(passphrase);
        addRecord({username, encryptPw(passphrase)});

        cout << "User created successfully.\n";
        main();
    } else if (query == 3) {
        cout << readDb() << "\n";
        main();
    } else if (query == 4) {
        resetDb();
        cout << "Database reset successfully.\n";
        main();
    } else if (query == 5) {
        return 0;
    } else {
        std::cout << "ERROR: Invalid query.\n";
        main();
    }
}
