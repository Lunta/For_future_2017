import urllib.parse
import urllib.request
from xml.dom.minidom import parseString

from Atmosphere.air_info import *


def SearchLocation(location_str):

    str = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/' \
          'getMsrstnAcctoRltmMesureDnsty?' \
          'stationName='+ urllib.parse.quote(location_str)+'&' \
          'dataTerm=month&' \
          'pageNo=1&' \
          'numOfRows=24&' \
          'ServiceKey=LkXZ0KCCJ4B52toUWxsA5xcobsnrNLOB55MHxbJbVBj1WUZ6P2BkN1Qka%2BytNClkxQbFC3mvvhxkI163p%2BTHjg%3D%3D&' \
          'ver=1.3'
    # 등급	    좋음	보통	나쁨	매우나쁨
    # Grade 값	1	    2	    3	    4
    # 항목	    SO2	    CO	        O3	    NO2	         PM10	PM2.5
    # 이름      아황산  일산화탄소  오존    이산화질소   미세    초미세
    # 단위	    ppm	    ppm	        ppm	    ppm	         ㎍/㎥	㎍/㎥

    request = urllib.request.Request(str)
    request.get_method = lambda: 'GET'
    response_body = urllib.request.urlopen(request).read().decode('utf-8')

    #print (response_body)
    tags = ['stationName', 'dataTime', 'so2Value', 'coValue', 'o3Value', 'no2Value', 'pm10Value', 'pm10Value24',
            'pm25Value', 'pm25Value24', 'so2Grade', 'coGrade', 'o3Grade', 'no2Grade', 'pm10Grade', 'pm25Grade']

    AtmosphereInfo = parseString(response_body)
    response = AtmosphereInfo.childNodes[0].childNodes
    body = response[3]
    item_list = body.childNodes[1].childNodes
    if item_list.length is 1:
        print('Fail to request.')
        print('Please search other name')
        return None
    dic = {}
    day_info = []
    for item in item_list:
        if item.nodeName == "item":
            subitems = item.childNodes
            for atom in subitems:
                if atom.nodeName in tags:
                    if atom.firstChild is None:
                        dic[atom.nodeName] = 'None'
                    else:
                        dic[atom.nodeName] = atom.firstChild.nodeValue
            day_info.append(dict(dic))

    return AirInfo(location_str, day_info)