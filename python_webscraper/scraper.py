#!/usr/bin/python3
import requests
import re
import base64
import json
import datetime

# DEBUG Start log time
print(f"[Starting generator: {datetime.datetime.now()}]")

#### ENDPOINTS ####
url1 = '[REDACTED]'
url2 = '[REDACTED]'

#### COMMON #######
token_string = '__RequestVerificationToken'

#### PIPELINE #####

# Attempt Reach the first site to gather required cookies
try:
      print(f"    [CONN] {url1}... ")
      init = requests.get(url1, timeout=7)
      if init.status_code != 200:
            raise requests.HTTPError
      print(f"    [SUCCESS] [{init.status_code}]")

      # Pull the XSRF cookie that is required for further action
      cookie_string = init.headers['set-cookie'].split(";")
      cookie = cookie_string[0].split("=")[1]

      # Use regex to find and pull the first token from the page
      formdata = re.findall("__RequestVerificationToken.*/>", init.text)[0].split('"')[4]

      # Build Request data
      header = {"Cookie": f"{token_string}={cookie}"}
      payload = {token_string: formdata}

      # DEBUG Visual print statement to verify header values good
      print(f"    [HEADER]: {header}")
      print(f"    [PAYLOAD]: {payload}")

      # Attempt Make POST request to get new token
      print(f"    [CONN] {url2}... ")
      final = requests.post(url2, headers=header, data=payload, timeout=7)
      print(f"    [SUCCESS] [{final.status_code}]")

      # The Access Key from the response payload
      key = final.text.strip('"')

      # DEBUG Print key returned
      print(f"    [TOKEN]: {key}")

      # DEBUG Visual to see expiry on token
      try:
            tmp = key.split(".")[1]+"="
            tmp_bytes = tmp.encode()
            tmp_details = base64.b64decode(tmp_bytes)
            details = json.loads(tmp_details.decode())
            print(f"    [EXP]: {datetime.datetime.fromtimestamp(int(details['exp']))}")
      except:
            print("     [B64ERROR]: Could not decode expiry")
      finally:
            print("Writing files... ", end="")
            key_file = open("service-admin-token.txt", "w")
            key_file.write(key)
            key_file.close()
            curl_file = open("curlable-token", "w")
            curl_file.write(f"Authorization: Bearer {key}")
            curl_file.close()
            print("Done!")
except requests.exceptions.HTTPError:
      try:
            print(f"    [ERROR]: {init.status_code}   ", end="")
      except:
            print("     [ERROR]: [-1]   ", end="")
      finally:
            print("There was an error making connection.")
except requests.exceptions.Timeout:
      print("     [TIMEOUT] The connection has timed out. Double check VPN settings or DNS.")
except requests.exceptions.ConnectionError:
      print("     [BADCONN] There was an error connecting to the service.")
except requests.exceptions.RequestException:
      print("     [NULL] There was an unusual problem")
finally:
      print(f"[Generator finished: {datetime.datetime.now()}]\n")
