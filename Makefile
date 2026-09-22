REPORT = ReportPshenichnyi
PROGRAM = solve

CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
LDLIBS = -lm

.PHONY: all report program clean rebuild

all: report program

report: $(REPORT).pdf

program: $(PROGRAM)

$(REPORT).pdf: report.tex preamble.sty
	latexmk -pdf -interaction=nonstopmode -halt-on-error \
		-jobname=$(REPORT) report.tex

$(PROGRAM): solve.c
	$(CC) $(CFLAGS) solve.c -o $(PROGRAM) $(LDLIBS)

clean:
	rm -f $(PROGRAM) \
		$(REPORT).pdf $(REPORT).aux $(REPORT).log $(REPORT).out \
		$(REPORT).toc $(REPORT).fls $(REPORT).fdb_latexmk \
		$(REPORT).synctex.gz

rebuild: clean all

