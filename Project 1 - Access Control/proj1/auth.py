import json
import sys

USER_FILE = "users.json"
DOMAIN_FILE = "domains.json"
TYPE_FILE = "types.json"
ACCESS_FILE = "access.json"

def add_user(name, password):
    if not name:
        handle_error("username missing")

    users = load_users()
    if name in users:
        handle_error("user exists")

    users[name] = password
    write_users(users)
    print("Success")

def authenticate(name, password):
    users = load_users()
    if not name in users:
        handle_error("no such user")

    if users[name] != password:
        handle_error("bad password")

    print("Success")


def set_domain(name, domain):
    if not domain:
        handle_error("missing domain")

    users = load_users()
    if not name in users:
        handle_error("no such user")

    domains = load_domains()
    if not domain in domains:
        domains[domain] = [name]
    elif name not in domains[domain]:
        domains[domain].append(name)
    write_domains(domains)

    print("Success")

def domain_info(domain):
    if not domain:
        handle_error("missing domain")

    domains = load_domains()
    if domain in domains:
        for user in domains[domain]:
            print(user)

def set_type(obj, typ):
    if not typ:
        handle_error("missing type")
    if not obj:
        handle_error("missing object")

    types = load_types()
    if not typ in types:
        types[typ] = [obj]
    elif obj not in types[typ]:
        types[typ].append(obj)
    write_types(types)

    print("Success")


def type_info(typ):
    if not typ:
        handle_error("missing type")

    types = load_types()
    if typ in types:
        for t in types[typ]:
            print(t)

# Creates a dictionary of dictionaries.
#
# Op1 {
#    Domain1 {
#        [type1, type2,...]
#    },
#    Domain2 {...}
# },
# Op2 {...}
def add_access(op, domain, typ):
    if not op:
        handle_error("missing operation")
    if not domain:
        handle_error("missing domain")
    if not typ:
        handle_error("missing type")

    domains = load_domains()
    types = load_types()
    permissions = load_access_rights()

    if op in permissions:
        domain_dict = permissions[op]
        if not domain in domain_dict:
            domain_dict[domain] = [typ]
        elif domain in domain_dict:
            if not typ in domain_dict[domain]:
                domain_dict[domain].append(typ)
    else:
        permissions[op] = dict()
        permissions[op][domain] = [typ]

    if not domain in domains:
        domains[domain] = []
        write_domains(domains)
    if not typ in types:
        types[typ] = []
        write_types(types)

    write_access_rights(permissions)
    print("Success")

def can_access(op, user, obj):
    if not op or not user or not obj:
        handle_error("access denied")

    users = load_users()
    domains = load_domains()
    types = load_types()
    perms = load_access_rights()

    if not op in perms:
        handle_error("access denied")

    # Find all domains the user belongs to
    user_domains = []
    for domain, user_list in domains.items():
        if user in user_list:
            user_domains.append(domain)

    # Find all types the object belongs to
    obj_types = []
    for typ, obj_list in types.items():
        if obj in obj_list:
            obj_types.append(typ)

    # If the user has no domains or the object has no types, fail
    if not user_domains or not obj_types:
        handle_error("access denied")

    # Check an operation against all the user domains, checking if that domain
    # has rights to perform the operation on the object's type(s)
    for domain in user_domains:
        if domain in perms[op]:
            for typ in obj_types:
                if typ in perms[op][domain]:
                    print("Success")
                    return

    print("access denied")

def load_users():
    try:
        with open(USER_FILE, "r") as infile:
            d = json.load(infile)
    except IOError:
        d = dict()
    return d

def write_users(users):
    with open(USER_FILE, "w") as outfile:
        json.dump(users, outfile, indent=4)

def load_domains():
    try:
        with open(DOMAIN_FILE, "r") as infile:
            d = json.load(infile)
    except IOError:
        d = dict()
    return d

def write_domains(domains):
    with open(DOMAIN_FILE, "w") as outfile:
        json.dump(domains, outfile, indent=4)

def load_types():
    try:
        with open(TYPE_FILE, "r") as infile:
            d = json.load(infile)
    except IOError:
        d = dict()
    return d

def write_types(types):
    with open(TYPE_FILE, "w") as outfile:
        json.dump(types, outfile, indent=4)

def load_access_rights():
    try:
        with open(ACCESS_FILE, "r") as infile:
            d = json.load(infile)
    except IOError:
        d = dict()
    return d

def write_access_rights(permissions):
    with open(ACCESS_FILE, "w") as outfile:
        json.dump(permissions, outfile, indent=4)

def perform_op(op):
    if op == "AddUser":
        validate_arg_count(op, 4)
        add_user(sys.argv[2], sys.argv[3])
    elif op == "Authenticate":
        validate_arg_count(op, 4)
        authenticate(sys.argv[2], sys.argv[3])
    elif op == "SetDomain":
        validate_arg_count(op, 4)
        set_domain(sys.argv[2], sys.argv[3])
    elif op == "DomainInfo":
        validate_arg_count(op, 3)
        domain_info(sys.argv[2])
    elif op == "SetType":
        validate_arg_count(op, 4)
        set_type(sys.argv[2], sys.argv[3])
    elif op == "TypeInfo":
        validate_arg_count(op, 3)
        type_info(sys.argv[2])
    elif op == "AddAccess":
        validate_arg_count(op, 5)
        add_access(sys.argv[2], sys.argv[3], sys.argv[4])
    elif op == "CanAccess":
        validate_arg_count(op, 5)
        can_access(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        handle_error("invalid command " + op)

def validate_arg_count(op, count):
    if len(sys.argv) < count:
        handle_error("too few arguments for " + op)
    if len(sys.argv) > count:
        handle_error("too many arguments for " + op)

def handle_error(msg):
    print("Error: " + str(msg))
    sys.exit()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        handle_error("missing command")
    perform_op(sys.argv[1])

