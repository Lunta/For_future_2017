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
    map_dict = dict()
    sidoInfo_dict = dict()
    averageInfo_dict = dict()
    SIDO_LIST = ['서울', '인천', '경기', '강원', '충북', '대전', '충남', '제주', '경북', '대구', '울산', '경남', '부산',
                 '전북', '광주', '전남', '세종']
    INFO_TAGS = ['stationName', 'dataTime', 'so2Value', 'coValue', 'o3Value', 'no2Value', 'pm10Value', 'pm10Value24',
                 'pm25Value', 'pm25Value24', 'so2Grade', 'coGrade', 'o3Grade', 'no2Grade', 'pm10Grade', 'pm25Grade']

    def __init__(self):
        self.query_sido_data()
        self.LoadMapFile()
        self.UpdateMapInfo('pm10Grade')

    def LoadMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'r', encoding='utf-8')
        map_file_str = map_file.read()
        map_file.close()

        self.map_info = parseString(map_file_str)
        area_list = self.map_info.childNodes[2].childNodes
        self.map_dict = dict()
        for area in area_list:
            if area.nodeName == 'g' or area.nodeName == 'path':
                id = area.attributes._attrs['id']._value
                self.map_dict[id] = []
                if id in '세종 ':
                    self.map_dict[id].append(area.attributes._attrs['fill'])
                subitems = area.childNodes
                for atom in subitems:
                    if atom.attributes is not None:
                        self.map_dict[id].append(atom.attributes._attrs['fill'])


    def LoadImage(self):
        if self.map_png is not None:
            del self.map_png
        self.map_png = PhotoImage(file="file.png")

    def SaveMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'w', encoding='utf-8')
        self.map_info.writexml(map_file)
        map_file.close()

    def MapFileToImage(self):
        drawing = svg2rlg('Administrative_divisions_map_of_South_Korea.svg')
        renderPM.drawToFile(drawing, "file.png")

    def UpdateMapInfo(self, kind_of_grade):
        for sido in self.sidoInfo_dict:
            num = len(self.sidoInfo_dict[sido])
            self.averageInfo_dict[sido] = dict()
            for tag in self.INFO_TAGS:
                self.averageInfo_dict[sido][tag] = 0
            for dic in self.sidoInfo_dict[sido]:
                for tag in self.INFO_TAGS:
                    if tag is not 'stationName' and tag is not 'dataTime':
                        try:
                            self.averageInfo_dict[sido][tag] += float(dic[tag])
                        except:
                            self.averageInfo_dict[sido][tag] += 0
                self.averageInfo_dict[sido]['stationName'] = sido
                self.averageInfo_dict[sido]['dataTime'] = dic['dataTime']

            for tag in self.INFO_TAGS:
                try:
                    self.averageInfo_dict[sido][tag] /= num
                    if 'Grade' in tag:
                        self.averageInfo_dict[sido][tag] = int(self.averageInfo_dict[sido][tag] + 0.5)
                except:
                    pass

        for sido in self.map_dict:
            for dic in self.map_dict[sido]:
                if self.averageInfo_dict[sido][kind_of_grade] is 1:
                    dic._value = '#00FF00'
                elif self.averageInfo_dict[sido][kind_of_grade] is 2:
                    dic._value = '#FFFF00'
                elif self.averageInfo_dict[sido][kind_of_grade] is 3:
                    dic._value = '#FF0000'
                elif self.averageInfo_dict[sido][kind_of_grade] is 4:
                    dic._value = '#000000'
        self.SaveMapFile()
        self.MapFileToImage()
        self.LoadImage()

    def query_sido_data(self):
        for sido in self.SIDO_LIST:
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
            #print(response_body)
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
                        if atom.nodeName in self.INFO_TAGS:
                            if atom.firstChild is None:
                                dic[atom.nodeName] = 'None'
                            else:
                                dic[atom.nodeName] = atom.firstChild.nodeValue
                    sido_info.append(dict(dic))

            self.sidoInfo_dict[sido] = sido_info








