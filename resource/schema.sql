CREATE TABLE messages (suuid INTEGER, duuid INTEGER, msg TEXT, timestamp INTEGER);
CREATE TABLE friendlist (frienduuid TEXT, name TEXT);
CREATE TABLE userlist(uuid INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, leasetime INTEGER, dormant INTEGER DEFAULT 1);
CREATE TABLE session(user TEXT, is_logged INTEGER);