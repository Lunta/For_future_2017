import urllib.parse
import urllib.request

str = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?' \
      'stationName='+urllib.parse.quote('부평구')+'&' \
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

print (response_body)
tags = ['dataTime', 'so2Value', 'so2Grade', 'coValue', 'coGrade', 'o3Value', 'o3Grade', 'no2Value', 'no2Grade',
        'pm10Value', 'pm10Grade', 'pm25Value', 'pm25Grade']
from xml.dom.minidom import *
from xml.dom.minidom import parse, parseString

AtmosphereInfo = parseString(response_body)
response = AtmosphereInfo.childNodes[0].childNodes
body = response[3]
print(body)
item_list = body.childNodes[1].childNodes
print(item_list.length)
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
        print(dic)

location_list = AtmosphereInfo.childNodes


map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'r', encoding='utf-8')
map_file_str = map_file.read()

MapInfo = parseString(map_file_str)
area_list = MapInfo.childNodes[2].childNodes
print(area_list.length)

for area in area_list:
    if area.nodeName == 'g' or area.nodeName == 'path':
        print(area.attributes._attrs['id']._value)
        subitems = area.childNodes
        for atom in subitems:
            if atom.attributes is not None:
                print(atom.attributes._attrs['id']._value + ' ' + atom.attributes._attrs['fill']._value + ' ' + atom.attributes._attrs['stroke']._value)
            #print(atom.nodeName)
            if atom.nodeName in tags:
                #print(atom.nodeName + ': ' + atom.firstChild.nodeValue)
                pass


#def launcherFunction(menu):
#    if menu == 'l':
#        BooksDoc = LoadXMLFromFile()
#    elif menu == 'b':
#        PrintBookList(["title", ])
#
#
#def PrintBookList(tags):
#    global BooksDoc
#    if not checkDocument():  # DOM이 None인지 검사
#        return None
#
#    booklist = BooksDoc.childNodes
#    book = booklist[0].childNodes
#    for item in book:
#        if item.nodeName == "book":  # 엘리먼트를 중 book인 것을 추출
#            subitems = item.childNodes  # book에 들어 있는 노드들을 가져옴
#            for atom in subitems:
#                if atom.nodeName in tags:
#                    print(＂title =＂, atom.firstChild.nodeValue)  # 책 목록을 출력
#

from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM

drawing = svg2rlg('Administrative_divisions_map_of_South_Korea.svg')

renderPM.drawToFile(drawing, "file.png")
