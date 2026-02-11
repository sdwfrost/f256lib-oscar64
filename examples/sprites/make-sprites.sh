#!/bin/bash


F256DIR=../..

IC=${F256DIR}/imageconvert


function makeSprite() {
	local NAME=$1

	${IC} sprite 32 assets/${NAME}.png

	mkdir -p generated
	mv assets/*.clut generated/.
	mv assets/*.indexed generated/.

	cat \
		generated/${NAME}-1x1.indexed \
		generated/${NAME}-2x1.indexed \
		generated/${NAME}-3x1.indexed \
		generated/${NAME}-4x1.indexed \
		generated/${NAME}-5x1.indexed \
		generated/${NAME}-1x2.indexed \
		generated/${NAME}-2x2.indexed \
		generated/${NAME}-3x2.indexed \
		generated/${NAME}-4x2.indexed \
		generated/${NAME}-5x2.indexed \
		> generated/${NAME}.sprite

	rm generated/*.indexed
}


makeSprite apache-left
makeSprite apache-left-front
makeSprite apache-right-front
makeSprite apache-right

cat \
	generated/apache-left.sprite \
	generated/apache-left-front.sprite \
	generated/apache-right-front.sprite \
	generated/apache-right.sprite \
	generated/apache-left.clut \
	> generated/sprites.bin
