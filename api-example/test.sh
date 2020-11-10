#!/bin/bash
curl -X POST -d @$2 https://$1.execute-api.eu-west-2.amazonaws.com/test/list 
