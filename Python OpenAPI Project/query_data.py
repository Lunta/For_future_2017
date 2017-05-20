import urllib.parse
import urllib.request

str = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?' \
      'stationName='+urllib.parse.quote('정왕동')+'&' \
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


from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM

drawing = svg2rlg('Administrative_divisions_map_of_South_Korea.svg')

renderPM.drawToFile(drawing, "file.png")
#import rsvg
#cairosvg.surface.PNGSurface=
#img = cairo.ImageSurface(cairo.FORMAT_ARGB32, 640,480)
#ctx = cairo.Context(img)
#
### handle = rsvg.Handle(<svg filename>)
## or, for in memory SVG data:
#handle= rsvg.Handle(None, 'Administrative_divisions_map_of_South_Korea.svg')
#
#handle.render_cairo(ctx)
#
#img.write_to_png("svg.png")