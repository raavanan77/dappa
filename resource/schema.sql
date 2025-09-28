CREATE TABLE messages (suuid INT, duuid INT, msg TEXT, timestamp INT);
CREATE TABLE friendlist (frienduuid TEXT, name TEXT);
CREATE TABLE userlist(username TEXT UNIQUE);
CREATE TABLE session(user TEXT, is_logged INT);