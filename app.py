# Project: image-uploader-808

print('Welcome to image-uploader-808')
# Python environment created in 'env' directory
# Packages installed: flask, flask_cors, pymongo

from flask import Flask, request, jsonify, render_template, send_from_directory
from flask_cors import CORS
app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, port=8000)
