import os
import requests
import subprocess
from getpass import getpass

def check_server(url):
    try:
        x = requests.head(url)
        return x.ok
    except requests.exceptions.MissingSchema:
        print('Make sure you specify the protocol (http / https) before the URL.')
        return False
    except requests.exceptions.ConnectionError:
        sdf
        return False

def decode_and_handle_error(content, is_binary):
    if not is_binary:
        content = content.decode()
        if content.startswith("error"):
            print(content)
            exit(1)

    return content

def get_file(url, is_binary):
    try:
        x = requests.get(url)
        content = x.content
    except 1:
        print('Failed to retrieve file from the server.')
        return None

    return decode_and_handle_error(content, is_binary)

def post_file(url, data, is_binary):
    try:
        x = requests.post(url, data)
        content = x.content
    except 1:
        print('Failed to send POST request and get content from the server.')
        return None

    return decode_and_handle_error(content, is_binary)

def get_hashes(content):
    user_hash = machine_hash = None

    for line in content.split('\n'):
        who, hash = line.split(' ')
        if who == 'user':
            user_hash = hash
        else:
            machine_hash = hash

    return user_hash, machine_hash

url = input('Enter the URL to the server: ')
url = url.rstrip('/')

print('Checking whether the server is accessible...')
ok = check_server(url)
if ok:
    print('Success.')
else:
    print('Error: could not connect to the server.')
    exit(1)

print('You now need to log into the server. '
      'Make sure you created an user there first.')

username = input('Username: ')
password = getpass('Password: ')

print('Logging in, retrieving user and machine hashes...')
content = get_file(url + '/api/new_machine.php?username=' + username
                   + '&password=' + password, False)

user_hash, machine_hash = get_hashes(content)

print('Creating directory, storing hashes in files...')
store_dir = 'shell_server_machine_' + machine_hash
os.makedirs(store_dir, exist_ok=True)
with open(os.path.join(store_dir, 'user_hash'), 'w') as f:
    f.write(user_hash)
with open(os.path.join(store_dir, 'machine_hash'), 'w') as f:
    f.write(machine_hash)

print('Downloading binary and getting version...')
binary = post_file(url + '/api/heartbeat.php?user=' + user_hash + "&machine="
                  + machine_hash, '0', True) # using version 0 to force update
index = binary.index(b'\n')
binary = binary[index + 1:]
index = binary.index(b'\n')
version = binary[:index].decode()
binary = binary[index + 1:]

print(f'Installing binary at version {version} ({len(binary)} bytes)...')
with open(os.path.join(store_dir, 'version'), 'w') as f:
    f.write(version)
bin_path = os.path.join(".", store_dir, 'shell_client')
with open(bin_path, 'wb') as f:
    f.write(binary)
os.chmod(bin_path, 0o755)

print('Starting daemon...')
subprocess.Popen(bin_path)

print('Successfully installed. '
      'You can now go back to your dashboard on the server.')
