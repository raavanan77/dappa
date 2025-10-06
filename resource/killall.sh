#!/bin/bash

sudo kill -9 $(ps -a | grep server.o | awk '{print $1}') &&
sudo kill -9 $(ps -a | grep client.o | awk '{print $1}') &&
ps -a