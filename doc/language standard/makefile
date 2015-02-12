##################################
# Standard of the eks language
# Copyright (C) 2015  Florian Evaldsson <florian.evaldsson@telia.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##################################

#this makefile can be done better.

NAME := doc
PDFTOOL := evince

#enable xelatex
XELATEX:=-e '$$pdflatex=q/xelatex %O -interaction=nonstopmode -halt-on-error %S/'


all:
	latexmk -pdf $(XELATEX) $(NAME).tex
	
run: all
	$(PDFTOOL) $(NAME).pdf
	
clean:
	rm $(NAME).pdf
	
cleanall:
	rm $(NAME).aux
	rm $(NAME).fdb_latexmk
	rm $(NAME).fls
	rm $(NAME).log
	rm $(NAME).pdf
