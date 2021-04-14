#!/usr/bin/env bash

GIT_REF=${$1:-develop}
docker build --build-arg GIT_REF=${GIT_REF} -t whitemech/lydia:latest .
