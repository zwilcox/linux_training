1. Install Remote SSH extension in vscode
2. Setup ssh keys with your VM
  * Check if you have an ssh key generated already on your host. `ls -la ~/.ssh`
  * If you don't run `ssh-keygen -t ed25519` and hit enter until it finishes
  * Once the key is genreated `cat ~/.ssh/id_ed25519.pub` and copy that to your clipboard
  * On your VM run `vim ~/.ssh/authorized_keys` and paste the key in there and save the file
  * chmod 600 ~/.ssh/* on your host 
3. Add a new remote host in vscode
  * Install remote-ssh extension
  * Click the remote explorer icon on the left side of vscode
  * Hit the plus button and enter `ssh student@172.16.1.<XYZ>`
  * Open the folder you want to work in on your VM
4. In your working directory created a .vscode directory and add the json `c_cpp_properties.json`
5. Make sure that C/C++ extension is installed in vscode on your VM
