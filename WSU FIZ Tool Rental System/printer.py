import barcode
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from reportlab.graphics.shapes import Drawing
import os

def generateBarcode(vid):
    ean = barcode.get('code128', vid)
    filename = ean.save('barcode')

    drawing = Drawing(80,70)
    # drawing = svg2rlg("barcode.svg")
    bar = svg2rlg("barcode.svg")
    drawing.add(bar)
    renderPDF.drawToFile(drawing, "barcode.pdf")
    os.startfile('barcode.pdf')

#generateBarcode("000000000002")

