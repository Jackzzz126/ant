#!/usr/bin/env python
#-*- coding: utf-8 -*-

import urllib2
#import cookielib
import hashlib

if __name__ == '__main__':
	url = {}
	data = {}
	secret_key = "g@bkf9n+g=*pdc!+_^6#@=-ls&i^y+mz1v!-_hxf%^76(@bd9#"

	url['login'] = r'/account/login/'
	pwd_md5 = hashlib.md5( "123456" ).hexdigest( )
	signature = hashlib.md5( "jack" + pwd_md5 + secret_key ).hexdigest( )
	data['login'] = r'{"username":"jack", "pwd_md5":"%s", "signature":"%s"}' % ( pwd_md5, signature )

	url['reg'] = '/account/register/'
	signature = hashlib.md5( "jack" + "123456" + secret_key ).hexdigest( )
	data['reg'] = r'{"username":"%s", "password":"123456", "signature":"%s"}' % ( "jack", signature )

	url['is_user_login'] = '/account/is_user_login/'
	data['is_user_login'] = r'{"userid":"1", "token":"123456"}'

	url['log_out'] = '/account/log_out/'
	data['log_out'] = r'{"userid":"1"}'

	url['test'] = '/hello'
	data['test'] = r'{"test":{"id":1, "name":"aaaaa", "equip":[1,2,3]}}'

	url['log'] = '/log/add'
	data['log'] = r'aaaaaaaaaaaaa aaaaaaaaaaaaaa aaaaaaaaaaaaa aaaaaaaaa aaaaaaaaaaaaa'

	site_url = 'http://192.168.11.61:9000'
	try:
		key = 'log'
		for i in xrange(0, 1000000):
			req = urllib2.Request( site_url + url[key], str(i) + "	" + data[key] )
			#req = urllib2.Request( site_url + url[key] )
			res = urllib2.urlopen( req )
			print site_url + url[key]
			print data[key]
			print ""
			print res.read( )
			res.close( )
	except IOError, e:
		print e
