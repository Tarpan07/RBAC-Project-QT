#include "User.h"

User::User(string name,string email,string role,string studentID,string passwordHash){

    this->name=name;
    this->email=email;
    this->role=role;
    this->studentID=studentID;
    this->passwordHash=passwordHash;

}

const string& User::getName() const { return name; }
const string& User::getEmail() const { return email; }
const string& User::getRole() const { return role; }
const string& User::getStudentID() const { return studentID; }
const string& User::getPasswordHash() const { return passwordHash; }