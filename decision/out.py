#!/usr/bin/python
import sys


def classify(record):
	if record[4]=="Some":
		return "Yes"
	elif record[4]=="Full":
		if record[3]=="Yes":
			if record[8]=="French":
				return "Yes"
			elif record[8]=="Thai":
				if record[2]=="No":
					return "No"
				elif record[2]=="Yes":
					return "Yes"
			elif record[8]=="Burger":
				return "Yes"
			elif record[8]=="Italian":
				return "No"
		elif record[3]=="No":
			return "No"
	elif record[4]=="None":
		return "No"

if __name__=='__main__':
	if len(sys.argv) != 2:
		print 'Usage: python classify.py [testfile]'
		sys.exit()
	testfile=sys.argv[1]
	lines = open(testfile).readlines()
	for line in lines:
		record = [field.strip() for field in line.split(',')]
		print 'Classifying:', record
		result=classify(record)
		print '-->', result
