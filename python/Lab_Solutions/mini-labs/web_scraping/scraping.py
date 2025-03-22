import base64
import os

# pip dependencies - requests (depends on urllib3), and bs4
import requests
import urllib
from bs4 import BeautifulSoup, Comment

## Make a GET request for the website
website = "http://127.0.0.1:8000"
response = requests.get(website)

if response.status_code != 200:
    raise Exception(f"Failed to reach {website}")

## Parse HTML data with BeautifulSoup
soup = BeautifulSoup(response.content, "html.parser")

## Find all the images and save them locally
image_dir = "Images"
if not os.path.exists(image_dir):
    os.makedirs(image_dir)
counter = 0
for img in soup.find_all("img"):
    src = img.get("src")
    if src.startswith("data:image"):
        header, img_data = src.split(",")
        img_type = header.split("/")[1].split(";")[0]
        file_name = f"Image{counter}.{img_type}"
        counter += 1
        img_data = base64.b64decode(img_data)
    else:
        url = urllib.parse.urljoin(website, src)
        # url = website + "/" + src
        response = requests.get(url)
        file_name = os.path.basename(src)
        img_data = response.content

    full_path = os.path.join(image_dir, file_name)

    with open(full_path, "wb") as fh:
        fh.write(img_data)


## Write all the comments to a file
file_name = "comments.txt"
with open(file_name, "w") as fh:
    for comment in soup.find_all(string=lambda x: isinstance(x, Comment)):
        fh.write(comment)
        fh.write("\n")
