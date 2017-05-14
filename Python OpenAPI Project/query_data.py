import urllib.parse
import urllib.request

url = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getCtprvnRltmMesureDnsty'
queryParams = '?&' + urllib.parse.urlencode({ urllib.parse.quote_plus('ServiceKey') : 'LkXZ0KCCJ4B52toUWxsA5xcobsnrNLOB55MHxbJbVBj1WUZ6P2BkN1Qka%2BytNClkxQbFC3mvvhxkI163p%2BTHjg%3D%3D',
                                             urllib.parse.quote_plus('stationName'): '종로구'.encode('utf-8'),
                                             urllib.parse.quote_plus('dataTerm'): 'month',
                                             urllib.parse.quote_plus('pageNo'): '1',
                                             urllib.parse.quote_plus('numOfRows'): '10'})


request = urllib.request.Request(url + queryParams)
request.get_method = lambda: 'GET'
response_body = urllib.request.urlopen(request).read()
print (response_body)

