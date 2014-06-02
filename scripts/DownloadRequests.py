#!/usr/bin/env python

# /****************************************************************************
# **
#  ** Copyright (C) 2011-2014 Andrey Kartashov .
#  ** All rights reserved.
#  ** Contact: Andrey Kartashov (porter@porter.st)
#  **
#  ** This file is part of the EMS web interface module of the genome-tools.
#  **
#  ** GNU Lesser General Public License Usage
#  ** This file may be used under the terms of the GNU Lesser General Public
#  ** License version 2.1 as published by the Free Software Foundation and
#  ** appearing in the file LICENSE.LGPL included in the packaging of this
#  ** file. Please review the following information to ensure the GNU Lesser
#  ** General Public License version 2.1 requirements will be met:
#  ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#  **
#  ** Other Usage
#  ** Alternatively, this file may be used in accordance with the terms and
#  ** conditions contained in a signed written agreement between you and Andrey Kartashov.
#  **
#  ****************************************************************************/

##
##
## Selects data from Wardrobe database with respect to status then downloads files from website
##
##

import os
import Settings
import requests
import re
import glob
import DefFunctions as d
import subprocess as s  # import call
import datetime
import urlparse
import urllib2
import shutil
import base64
import string

print str(datetime.datetime.now())

settings = Settings.Settings()

#EDB = settings.settings['experimentsdb']
WARDROBEROOT = settings.settings['wardrobe']
PRELIMINARYDATA = WARDROBEROOT + '/' + settings.settings['preliminary']
TEMP = WARDROBEROOT + '/' + settings.settings['temp']
BIN = WARDROBEROOT + '/' + settings.settings['bin']

extension = 'fastq'


######################################################################
def check_auth(url, auth):
    lp = auth.split('@')[0]
    username = lp.split(':')[0]
    password = lp.split(':')[1]
    url = url.replace(lp + '@', '')
    req = urllib2.Request(url)
    try:
        r = urllib2.urlopen(req)
    except IOError, e:
        pass
    if hasattr(e, 'code') and e.code == 401 and 'basic' in e.headers.get('www-authenticate', '').lower():
        base64string = base64.encodestring('%s:%s' % (username, password))[:-1]
        authheader = "Basic %s" % base64string
        req.add_header("Authorization", authheader)
        try:
            r = urllib2.urlopen(req)
        except IOError, e:
            return ['Warning', 'Wrong authorization ' + url]
            pass
    else:
        return ['Warning', 'Unsupported authorization ' + e.headers.get('www-authenticate', '').lower()]
        pass
    return ['', r]


######################################################################
def decompress(fname, ofname):
    ofname=string.replace(ofname, "//", "/")
    cmd = ""
    if re.search("\.gz$", fname):
        cmd = 'zcat "' + ofname + '.gz" >>' + ofname + '; rm -f ' + ofname + '.gz'
        ofname += '.gz'
    elif re.search("\.bz2$", fname):
        cmd = 'bzcat "' + ofname + '.bz2" >>' + ofname + '; rm -f ' + ofname + '.bz2'
        ofname += '.bz2'
    elif re.search("\.zip$", fname):
        cmd = 'unzip -p "' + ofname + '.zip" >>' + ofname + '; rm -f ' + ofname + '.zip'
        ofname += '.zip'
    elif re.search("\.sra$", fname):
        cmd = 'fastq-dump -Z -B "' + ofname + '.sra" >>' + ofname + ' && rm -f ' + ofname + '.sra'
        ofname += '.sra'
    else:
        cmd = 'cat "' + ofname + '.part" >>' + ofname + '; rm -f ' + ofname + '.part'
        ofname += '.part'
    return [cmd, ofname]


######################################################################
def get_file_url(urlin, basedir, filename, pair, force=False):

    if not force and os.path.isfile(basedir + '/' + filename + '.' + extension):
        return ['Error', 'File already exist, possible mistake when download']

    flist = list()
    ofname = str()
    urls = urlin.split(';')

    for url in urls:
        if len(url) == 0:
            continue
        urlparsed = urlparse.urlparse(url)
        if ('http' not in urlparsed[0]) and ('ftp' not in urlparsed[0]):
            return ['Warning', 'ftp and http methods are supported']

        if ('@' in urlparsed[1]) and ('http' in urlparsed[0]):
            rc = check_auth(url, urlparsed[1])
            if rc[0] != '':
                return rc
            r = rc[1]
        else:
            r = urllib2.urlopen(url)

        # If the response has Content-Disposition, we take file name from it
        if r.info().has_key('Content-Disposition'):
            fname = r.info()['Content-Disposition'].split('filename=')[1]
            if fname[0] == '"' or fname[0] == "'":
                fname = fname[1:-1]
        else:
            fname = os.path.basename(urlparse.urlparse(r.url)[2])

        if len(fname) == 0:
            fname = "default_fastq"

        if 'fastq' not in fname and 'sra' not in fname:
            return ['Warning', 'File has to contain fastq string']

    if not pair:
        ofname = basedir + '/' + filename + '.' + extension

        (cmd, ofname) = decompress(fname, ofname)

        for url in urls:
            urlparsed = urlparse.urlparse(url)
            if ('@' in urlparsed[1]) and ('http' in urlparsed[0]):
                rc = check_auth(url, urlparsed[1])
                if rc[0] != '':
                    return rc
                r = rc[1]
            else:
                r = urllib2.urlopen(url)

            try:
                with open(ofname, 'wb') as f:
                    shutil.copyfileobj(r, f)
            except Exception, e:
                return ['Warning', 'Cant download from ' + url]
            finally:
                r.close()

            try:
                s.check_output(cmd, shell=True)
            except Exception, e:
                return ['Warning', 'Cant uncompress ' + ofname]
        flist.append(filename)
        return flist

    return ['Warning', 'Cant find file']


#### end of def get_file

######################################################################
def get_file_core(USERNAME, PASSWORD, libcode, basedir, filename, pair):
    if os.path.isfile(BIN + '/Core.py'):
        with open(BIN + '/Core.py', 'r') as content_file:
            content = content_file.read()
        exec content
        return get_core(USERNAME, PASSWORD, libcode, basedir, filename, pair)
    else:
        return ['Warning', 'Core module not supported']


######################################################################
def local_file(downloaddir,basedir, libcode, filename, pair):
    fl = glob.glob(downloaddir + '/' + libcode)
    if len(fl) == 0:
        fl = glob.glob(downloaddir + '/*' + libcode + '*')

    flist = list()

    if len(fl) > 0 and len(fl) != 2 and pair:
        return ['Error', 'incorrect number of files for pair end reads']
    if len(fl) > 0 and len(fl) != 1 and not pair:
        return ['Error', 'incorrect number of files for single end reads']
    c = 0
    for i in fl:
        if c == 0:
            os.rename(i, basedir + '/' + filename + '.' + extension)
            flist.append(filename + '.' + extension)
            c = 1
        else:
            os.rename(i, basedir + '/' + filename + '_2.' + extension)
            flist.append(filename + '_2.' + extension)
    return flist


######################################################################
def get_file_local(downloaddir,basedir, libcode, filename, pair):
    return ""


######################################################################

######################################################################

pidfile = "/tmp/DownloadRequests.pid"
d.check_running(pidfile)


#in future if exist stus 1 delete temporary donloaded files
settings.cursor.execute("UPDATE labdata SET libstatus=2000 WHERE libstatus IN (1)")
settings.cursor.execute("UPDATE labdata SET libstatus=0 WHERE libstatus IN (1000)")
settings.conn.commit()

while True:
    row = []
    settings.cursor.execute(
        "SELECT dnalogin,dnapass,l.url,l.uid, etype,w.email,w.notify,l.download_id,w.id,forcerun "
        " FROM labdata l, experimenttype e, worker w "
        " WHERE l.worker_id = w.id AND e.id=experimenttype_id "
        " AND (( COALESCE(dnalogin,'') <> '' AND COALESCE(dnapass,'') <> '' AND l.download_id = 1) OR l.download_id > 1 ) "
        " AND deleted=0 AND COALESCE(url,'') <> '' AND libstatus IN (0) LIMIT 1")
    row = settings.cursor.fetchone()
    if not row:
        break

    notify = (int(row[6]) == 1)
    UID = row[3]
    url = row[2]
    downloadid = int(row[7])
    email = row[5]
    forcerun = (int(row[9]) == 1)
    PAIR = ('pair' in row[4])

    basedir = PRELIMINARYDATA + '/' + UID
    try:
        os.makedirs(basedir, 0777)
    except:
        pass
    try:
        os.chmod(basedir, 0777)
    except:
        pass

    a = list()
    if downloadid == 1:
        settings.cursor.execute("update labdata set libstatustxt=%s,libstatus=1 where uid=%s", ('downloading', UID))
        settings.conn.commit()
        a = get_file_core(row[0], row[1], url, basedir, UID, PAIR)
    if downloadid == 2:
        settings.cursor.execute("update labdata set libstatustxt=%s,libstatus=1 where uid=%s",
                                ("URL downloading in proccess", UID))
        settings.conn.commit()
        a = get_file_url(url, basedir, UID, PAIR, forcerun)
    if downloadid == 3:
        settings.cursor.execute("update labdata set libstatustxt=%s,libstatus=1000 where uid=%s",
                                ("Not supproted yet!", UID))
        settings.conn.commit()
        continue

    if 'Error' in a[0]:
        settings.cursor.execute("update labdata set libstatustxt=%s,libstatus=2000 where uid=%s",
                                (a[0] + ":" + a[1], UID))
        settings.conn.commit()
        if 'incorrect DNA core credentials' in a[1]:
            settings.cursor.execute("update worker set dnapass='' where id = %s", (row[8]))
            settings.conn.commit()
        continue
    if 'Warning' in a[0]:
        settings.cursor.execute("update labdata set libstatustxt=%s,libstatus=1000 where uid=%s",
                                (a[0] + ":" + a[1], UID))
        settings.conn.commit()
        continue

    settings.cursor.execute("update labdata set libstatustxt='downloaded',libstatus=2,filename=%s where uid=%s",
                            (UID, UID))
    settings.conn.commit()
    if notify:
        try:
            d.send_mail(email, 'Record #' + str(UID) + ' has been downloaded')
        except:
            pass

settings.cursor.close()
