# test_TableReadFromXLSXFile.praat
# djmw 20261126

appendInfoLine: "test_TableReadFromXLSXFile.praat"

@test_readFromFile
@test_readFromFileWithError

procedure test_readFromFileWithError
	appendInfo: tab$, "test_readFromFileWithError:"
	asserterror In row 7 the number of cells is larger than the number of columns (4).
	table = Read from file: "TableFile_bad.xlsx"
	appendInfoLine: " OK"
endproc

procedure test_readFromFile
	appendInfo: tab$, "test_readFromFile:"
	table = Read from file: "TableFile_good.xlsx"
	nrow = Get number of rows
	assert nrow = 303; 'ncol'
	ncol = Get number of columns
	assert ncol = 4; 'ncol'
	columnLabel$# = {"Participant ID", "Gender", "Age", "Diagnosis"}
	for icol to 4
		label$ = Get column label: icol
		columnLabel$ = columnLabel$# [icol]
		assert columnLabel$ = label$; 'icol'
	endfor

	# Check first and last row of table
	row1$# = {"BL01","M","46"}
	row297$# = {"Sj30008","Female","18"}

	for icol to 3
		val$ = Get value: 1, columnLabel$# [icol]
		val1$ = row1$# [icol]
		assert val$ = val1$; 'val$' <> 'val1$' 'icol'
		val$ = Get value: 297, columnLabel$# [icol]
		val1$ = row297$# [icol]
		assert val$ = val1$; 'val$' <> 'val1$' 'icol'
	endfor
	removeObject: table
	appendInfoLine: " OK"
endproc

appendInfoLine: "test_TableReadFromXLSXFile.praat: OK"