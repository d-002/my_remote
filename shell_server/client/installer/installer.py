import os
import requests
import subprocess
from urllib.parse import urlparse
from getpass import getuser, getpass

# make the server think we are not a bot
headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}

def check_server(url: str) -> tuple[str | None, str | None]:
    try:
        x = requests.head(url, stream=True, headers=headers)
        if x.ok:
            parsed = urlparse(x.url)
            return parsed.hostname, str(parsed.port) if parsed.port else '80'
        return None, None
    except requests.exceptions.MissingSchema:
        print('Make sure you specify the protocol (http / https) before the URL.')
        return None, None
    except requests.exceptions.ConnectionError:
        return None, None

def decode_and_handle_error(content: bytes) -> bytes:
    if content.startswith(b"error"):
        print(content.decode())
        exit(1)

    return content

def get_file(url: str) -> bytes | None:
    try:
        x = requests.get(url, headers=headers)
        content = x.content
        print(content)
    except:
        print('Failed to retrieve file from the server.')
        return None

    return decode_and_handle_error(content)

def post_file(url: str, data: str | bytes) -> bytes | None:
    try:
        x = requests.post(url, data, headers=headers)
        content = x.content
    except:
        print('Failed to send POST request and get content from the server.')
        return None

    return decode_and_handle_error(content)

def get_hashes(content: str) -> tuple[str | None, str | None]:
    user_hash = machine_hash = None

    for line in content.split('\n'):
        who, hash = line.split(' ')
        if who == 'user':
            user_hash = hash
        else:
            machine_hash = hash

    return user_hash, machine_hash

url = input('Enter the URL (and port if needed) to the server: ')
url = url.rstrip('/')

print('Checking whether the server is accessible...')
host, port = check_server(url + '/')
if type(host) == str and type(port) == str:
    print('Success.')
else:
    print('Error: could not connect to the server.')
    exit(1)

print('\nYou now need to log into the server.')
print('Make sure you created an user there first.')

username = input('Username: ')
password = getpass('Password: ')

user = getuser()
user_safe = ''.join(c if c.isalnum() else '_' for c in user)

print('\nLogging in, retrieving user and machine hashes...')
content = get_file(url + '/api/new_machine.php?username=' + username
                   + '&password=' + password + '&name=' + user_safe)
if (type(content) != bytes):
    print('Error while retrieving hashes')
    exit(1)
content = content.decode()

user_hash, machine_hash = get_hashes(content)
if (type(user_hash) != str or type(machine_hash) != str):
    print('Error while retrieving hashes')
    exit(1)

print('Creating directory, storing hashes and server settings in files...')
store_dir = 'shell_server_machine_' + machine_hash

os.makedirs(store_dir, exist_ok=True)
with open(os.path.join(store_dir, 'user_hash'), 'w') as f:
    f.write(user_hash)
with open(os.path.join(store_dir, 'machine_hash'), 'w') as f:
    f.write(machine_hash)
with open(os.path.join(store_dir, 'host'), 'w') as f:
    f.write(host)
with open(os.path.join(store_dir, 'port'), 'w') as f:
    f.write(port)

print('\nDo you wish to set up the software and version manually?')
print('If you answer "no", this utility will query and run the software '
      'specified in the user\'s dashboard.')
manual_install = 'y' in input('[y/N]: ')
print()

bin_path = os.path.join(".", store_dir, 'shell_client')
version_path = os.path.join(".", store_dir, 'version')

if manual_install:
    print('You chose the manual install.')
    print('You will need to place :')
    print(f'- A valid binary in {bin_path}')
    print(f'- A valid version in {version_path}')
    print('And then run the software.')
else:
    print('Downloading binary and getting version from the user...')
    # using version 0 to force update
    res = post_file(url + '/api/heartbeat.php?user=' + user_hash +
                       "&machine=" + machine_hash, '0')
    if (res == None):
        print('Failed to download binary.')
        exit(1)

    index = res.index(b'\n')
    binary = res[index + 1:]
    index = binary.index(b'\n')
    version = binary[:index].decode()
    binary = binary[index + 1:]

    print(f'Installing binary at version {version} ({len(binary)} bytes)...')
    with open(version_path, 'w') as f:
        f.write(version)
    with open(bin_path, 'wb') as f:
        f.write(binary)
    os.chmod(bin_path, 0o755)

    print('Starting software...')
    subprocess.Popen(bin_path)

    print('\nSuccessfully installed.')
    print('You can now go back to your dashboard on the server.')
