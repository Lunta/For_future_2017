from xml.dom.minidom import *
from xml.dom.minidom import parse, parseString
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM
from tkinter import *
import urllib.parse
import urllib.request

class InfoMap:
    map_info = None
    map_png = None
    location_list = []
    sido_dict = dict()

    def __init__(self):
        self.LoadMapFile()
        print(self.location_list)
        print(len(self.location_list))
        self.query_sido_data()

    def LoadMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'r', encoding='utf-8')
        map_file_str = map_file.read()
        map_file.close()

        self.map_info = parseString(map_file_str)
        area_list = self.map_info.childNodes[2].childNodes
        print(area_list.length)

        for area in area_list:
            if area.nodeName == 'g' or area.nodeName == 'path':
                print(area.attributes._attrs['id']._value)
                self.location_list.append(area.attributes._attrs['id']._value)
                if area.attributes._attrs['id']._value in '세종특별자치시':
                    print(area.attributes._attrs['fill']._value + ' ' + area.attributes._attrs['stroke']._value)
                    area.attributes._attrs['fill']._value = '#000000'
                subitems = area.childNodes
                for atom in subitems:
                    if atom.attributes is not None:
                        try:
                            print(atom.attributes._attrs['id']._value + ' ' + atom.attributes._attrs['fill']._value + ' ' + atom.attributes._attrs['stroke']._value)
                            self.location_list.append(atom.attributes._attrs['id']._value)
                        except:
                            try:
                                print(atom.attributes._attrs['id']._value)
                                self.location_list.append(atom.attributes._attrs['id']._value)
                            except:
                                print(atom.attributes._attrs['fill']._value + ' ' + atom.attributes._attrs['stroke']._value)

                            if atom.nodeName == 'g' or atom.nodeName == 'path':
                                subatoms = atom.childNodes
                                for part in subatoms:
                                    if part.attributes is not None:
                                        print(part.attributes._attrs['fill']._value + ' ' + part.attributes._attrs['stroke']._value)

    def LoadImage(self):
        self.map_png = PhotoImage(file="file.png")

    def SaveMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'w', encoding='utf-8')
        self.map_info.writexml(map_file)
        map_file.close()

    def MapFileToImage(self):
        drawing = svg2rlg('Administrative_divisions_map_of_South_Korea.svg')
        renderPM.drawToFile(drawing, "file.png")

    def UpdateMapInfo(self):
        pass

    def query_sido_data(self):
        sido_list = ['서울', '부산', '대구', '인천', '광주', '대전', '울산', '경기', '강원', '충북', '충남', '전북',
                     '전남', '경북', '경남', '제주', '세종']
        for sido in sido_list:
            str = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/' \
                  'getCtprvnRltmMesureDnsty?' \
                  'sidoName=' + urllib.parse.quote(sido) + '&' \
                  'pageNo=1&' \
                  'numOfRows=10000&' \
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

            tags = ['stationName', 'dataTime', 'so2Value', 'coValue', 'o3Value', 'no2Value', 'pm10Value', 'pm10Value24',
                    'pm25Value', 'pm25Value24', 'so2Grade', 'coGrade', 'o3Grade', 'no2Grade', 'pm10Grade', 'pm25Grade',
                    'pm10Grade1H', 'pm25Grade1H']

            AtmosphereInfo = parseString(response_body)
            response = AtmosphereInfo.childNodes[0].childNodes
            body = response[3]
            item_list = body.childNodes[1].childNodes

            dic = {}
            sido_info = []
            for item in item_list:
                if item.nodeName == "item":
                    subitems = item.childNodes
                    for atom in subitems:
                        if atom.nodeName in tags:
                            if atom.firstChild is None:
                                dic[atom.nodeName] = 'None'
                            else:
                                dic[atom.nodeName] = atom.firstChild.nodeValue
                    sido_info.append(dict(dic))

            self.sido_dict[sido] = sido_info
        for d in self.sido_dict:
            print(d, self.sido_dict[d])



