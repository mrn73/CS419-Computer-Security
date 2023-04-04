Project 1: Access Control

Overview:
    Stored in this folder is auth.py.
    As new users, domains, types, and access rights are created,
    new json files will be added in this directory.
        users.json --> stores users and their passwords
        domains.json --> stores domains and their lists of users
        types.json --> stores types and their list of objects
        access.json --> stores operations with their list of domains,
                        where domains store a list of types they can act on

How to use:
    python3 auth.py [command] {args}

    commands(args):
        AddUser("user_name", "password")
        Authenticate("user_name", "password")
        SetDomain("user_name", "domain_name")
        DomainInfo("domain_name")
        SetType("object_name", "type_name")
        TypeInfo("type_name")
        AddAccess("operation", "domain", "type_name")
        CanAccess("operation", "user", "object")

----------------------------------------------------------------------------
Test Cases:
    1. AddUser
        a. input: AddUser michael ""
           output: Success
        b. input: AddUser michael password
           output: Error: user exists
        c. input: AddUser "" ""
           ouput: Error: username missing
    2. Authenticate
        a. input: Authenticate michael ""
           ouput: Success
        b. input: Authenticate michael abc
           output: Error: bad password
        c. input: Authenticate matt password
           output: Error: no such user
    3. SetDomain
        a. input: SetDomain michael admin
           output: Success
                   *admin now exists in domains.json*
        b. input: SetDomain michael admin
           output: Success
                   *no duplicate "michael" created*
        c. input: SetDomain michael ""
           ouput: Error: missing domain
        d. input: SetDomain matt admin
           output: Error: no such user
    4. DomainInfo
        a. input: DomainInfo admin
           output: michael
        b. input: DomainInfo ""
           output: Error: missing domian
        c. input: DomainInfo nodomain
           output:
    5. SetType
        a. input: SetType compsec class
           output: Success
        b. input: SetType compsec class
           output: Success
                   *no duplicate "compsec" created*
        c. input: SetType compsec ""
           output: Error: missing type
        d. input: SetType "" class
           output: Error: missing object
    6. TypeInfo
        a. input: TypeInfo class
           output: compsec
        b. input: TypeInfo movie
           output:
        c. input: TypeInfo ""
           output: Error: missing type
    7. AddAccess
        * These test cases begin with there only being michael as user.
        * There are no domains or types stored yet.
        a. input: AddAccess add student course
           output: Success
                   *Created "student" domain and "course" type*
        b. input: AddAccess "" student course
           output: Error: missing operation
        c. input: AddAccess delete "" course
           output: Error: missing domain
        d. input: AddAccess delete student ""
           output: Error: missing type
    8. CanAccess
        * These test cases build off the previous section.
        * Added michael to "student" domain and compsec to "course" type.
        a. input: CanAccess add michael compsec
           output: Success
        b. input: CanAccess add michael ai
           output: Error: access denied
                   *ai not stored in "course" type*
        c. input: CanAccess "" michael compsec
           output: Error: access denied
        d. input: CanAccess add "" compsec
           output: Error: access denied
        e. input: CanAccess add michael ""
           output: Error: access denied

