from pymongo import MongoClient

# MongoDB connection string
MONGO_URI = 'mongo_url_here'

# Database name
DB_NAME = 'mydb'

def get_db():
    client = MongoClient(MONGO_URI)
    return client[DB_NAME]

def close_db(client):
    client.close()
