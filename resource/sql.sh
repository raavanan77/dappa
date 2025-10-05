rm /home/raavanan/.config/dappa/storage.db
touch /home/raavanan/.config/dappa/storage.db
sqlite3 /home/raavanan/.config/dappa/storage.db < ~/dappa/resource/schema.sql