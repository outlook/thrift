#!/usr/bin/env python

import json
import os
import sys
import urllib

try:
    import requests
    from requests.auth import HTTPBasicAuth
except:
    print "Cannot import requests - please 'pip install requets'"
    raise


class GithubRelease:
    def __init__(self, name, user, token):
        self.name = name
        self.auth = HTTPBasicAuth(user, token)
        

    def create_with_attachments(self, attachments=[]):
        for a in attachments:
            if not os.path.isfile(a):
                raise Exception('%s is not a valid file' % a)

        url = 'https://api.github.com/repos/Outlook/thrift/releases'
        payload = json.dumps({
            'tag_name': self.name,
            'name': self.name,
            'draft': len(attachments) > 0,
            })

        resp = requests.post(url, auth=self.auth, data=payload)
        resp.raise_for_status()

        if not attachments:
            return

        body = resp.json()
        upload_url = body['upload_url']
        if '{' in upload_url:
            upload_url = upload_url[:upload_url.find('{')]
            print "NEW UPLOAD URL: " + upload_url

        for filename in attachments:
            headers = {
                    'Content-Type': 'application/octet-stream'
            }
            name = filename.split('/')[-1]
            asset_url = upload_url + '?' + urllib.urlencode({'name': name})
            print "UPLOADING TO: " + asset_url
            with open(filename, 'rb') as f:
                r = requests.post(
                        asset_url,
                        auth=self.auth,
                        headers=headers,
                        data=f)
                r.raise_for_status()


def require_token():
    if not os.path.isfile('~/.gh_token'):
        raise Exception('Github token required at ~/.gh_token in the form of "username:token"')


if __name__ == '__main__':
    tag = sys.argv[1]
    user = sys.argv[2]
    token = sys.argv[3]
    attachment = sys.argv[4]

    GithubRelease(tag, user, token).create_with_attachments([attachment])

