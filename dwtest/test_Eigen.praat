# test_Eigen.praat
# djmw 20161116, 20180829, 20210609, 20260314

goto veryEnd
appendInfoLine: "test_Eigen.praat:"

@testOlderFormats
include readingAndWritingOfObjects.praat
;@testReadAndWrite

@testInterface
eps = 1e-7
@testDiagonals: 10

@testSymmetric_2by2
@testSymmetric_3by3
@testSymmetric_4by4
@testGeneral_3by3

appendInfoLine: "test_Eigen.praat: OK"	

procedure testOlderFormats
	.test$ = tab$ + "test older formats:"
	appendInfoLine: .test$
	.test2$ = tab$ + tab$ + "format 0:"
	appendInfoLine: .test2$
	.eigen = Read from file: "3x3s_format0.Eigen"
	.eigenvalues# = {11, 2, 1}
	.eigenvectors## = {{0, 0.4472135954999579,0.8944271909999159},
		... {1, 0, 0},
		... {0, -0.8944271909999159, 0.4472135954999579}}
	@testEqualityOfEigenvalues: .eigen, .eigenvalues#
	@testEqualityOfEigenvectors: .eigen, .eigenvectors##
	removeObject: .eigen
	appendInfoLine: .test2$, " OK"
	appendInfoLine: .test$ + " OK"

endproc

procedure testReadAndWrite
	.eigen = Read from file: "3x3s_format0.Eigen"
	@testReadingAndWritingOfObject: .eigen, "Eigen",
	removeObject: .eigen
endproc

procedure appendInterfaceName: .i, .text$
	if  .i = 1
		appendInfoLine: tab$, tab$, .text$
	endif
endproc

procedure testInterface
	appendInfoLine: tab$, "interface test:"
	for .i to 5
		.numberOfColumns = randomInteger (3, 12)
		.tableofreal = Create TableOfReal: "t", 100, .numberOfColumns
		Formula: ~ randomGauss (0, 1)
		.pca = To PCA
		.eigen = Extract Eigen
		@appendInterfaceName: .i, "Get number of eigenvalues"
		.numberOfEigenvalues = Get number of eigenvalues
		assert .numberOfEigenvalues == .numberOfColumns
		@appendInterfaceName: .i, "Get eigenvector dimension"
		.dimension = Get eigenvector dimension
		assert .dimension == .numberOfColumns
		for .j to .numberOfEigenvalues
			@appendInterfaceName: .i, "Get eigenvalue: "+ string$ (.j)
			.eigenvalue [.j] = Get eigenvalue: .j
		endfor
		for .j to .numberOfEigenvalues
			.sump = Get eigenvalue: .j
			for .k from .j to .numberOfEigenvalues
				@appendInterfaceName: .i, "Get sum of eigenvalues: " + string$ (.j) + ", " + string$ (.k)
				.sum = Get sum of eigenvalues: .j, .k
				assert .sum >= .eigenvalue [.j]
			endfor
		endfor

		for .j to .numberOfEigenvalues
			for .k from .j to .dimension
				@appendInterfaceName: .i, "Get eigenvector element: " + string$ (.j) + ", " + string$ (.k)
				.val[.j,.k] = Get eigenvector element: .j, .k
			endfor
		endfor
		for .j to .numberOfEigenvalues
			for .k to .dimension
				.val[.k] = Get eigenvector element: .j, .k
			endfor
			@appendInterfaceName: .i, "Invert eigenvector: " + string$ (.j)
			Invert eigenvector: .j
			for .k to .dimension
				.valk = Get eigenvector element: .j, .k
				assert .valk == - .val[.k]
			endfor	
		endfor
		removeObject: .tableofreal, .pca, .eigen
	endfor
	appendInfoLine: tab$, "interface test: OK"
endproc

procedure assertApproximatelyEqual: .val1, .val2, .eps, .comment$
	.diff = abs (.val1 -.val2)
	.tekst$ = .comment$ + " " + string$ (.val1) + ", " + string$ (.val2)
	if .val1 == 0
		assert .diff < .eps; '.tekst$'
	else
		.reldif =  .diff / abs(.val1)
		assert .reldif < .eps ; '.tekst$'
	endif
endproc

procedure testSymmetric_2by2
	.test$ = tab$ + "2x2 symmetrical:"
	appendInfoLine: .test$
	.dim = 2
	.mat## = {{2, 1},
	...			  {1, 2}}
	.eigenvalues# = {3, 1}
	.mat = Create simple Matrix: "2x2s", .dim, .dim, ~ .mat##[row,col]
	# (2-z)^2 - 1 = 0 => eigenvalues z are 3 and 1
	# |2 1| |x|   |2x+y|      |x|  z = 3 => y =  x  (= 1/sqrt(2))
	# |1 2| |y| = |x+2y| =  z |y|  z=1   => y = -x
	.eigenvalues# = {3, 1}
	.eigen = To Eigen
	@testEqualityOfEigenvalues: .eigen, .eigenvalues#
	@testOrthogonalityOfEigenvectors: .eigen
	removeObject: .mat, .eigen
	appendInfoLine: .test$, " OK"
endproc

procedure testSymmetric_3by3
	.test$ = tab$ + "3x3 symmetrical:"
	appendInfoLine: .test$
	.dim = 3
	.mat## = {{2, 0, 0},
		...		  {0, 3, 4},
		...		  {0, 4, 9}}
	.eigenvalues# = {11, 2 , 1}
	.mat = Create simple Matrix: "3x3s", .dim, .dim, ~ .mat##[row,col]
	.eigen = To Eigen
	@testEqualityOfEigenvalues: .eigen, .eigenvalues#
	@testOrthogonalityOfEigenvectors: .eigen
	removeObject: .mat, .eigen
	appendInfoLine: .test$, " OK"
endproc

procedure testSymmetric_4by4
	.test$ = tab$ + "4x4 symmetrical:"
	appendInfoLine: .test$
	.dim = 4
	.mat## = {{1, 2, 3, 4},
	...			  {2, 3, 4, 6},
	...				{3, 4, 6, 5},
	...				{4, 6, 5, 7}}
	.eigenvalues# =	{17.333479094364993, 1.758549986173555, -0.24923615375564823, -1.8427929267829193}
	.mat = Create simple Matrix: "4x4s", .dim, .dim, ~ .mat##[row,col]
	.eigen = To Eigen (special): "symmetric", 0, "no"
	@testEqualityOfEigenvalues: .eigen, .eigenvalues#
	@testOrthogonalityOfEigenvectors: .eigen
	removeObject: .mat, .eigen
	appendInfoLine: .test$, " OK"
endproc

procedure testEqualityOfEigenvalues_im: .eigen, .givenValues#
	appendInfo: tab$, tab$, tab$, "equality of eigenvalues (imag):"
	selectObject: .eigen
	.eigenvalues# = List eigenvalues (imag)
	assert size (.eigenvalues#) = size (.givenValues#)
	for .ival to size (.eigenvalues#)
		@assertApproximatelyEqual: .eigenvalues# [.ival], .givenValues# [.ival], 1e-13, "testEqualityOfEigenvalues (imag)"
	endfor
	appendInfoLine: " OK"	
endproc

procedure testEqualityOfEigenvalues: .eigen, .givenValues#
	appendInfo: tab$, tab$, tab$, "equality of eigenvalues:"
	selectObject: .eigen
	.eigenvalues# = List eigenvalues
	assert size (.eigenvalues#) = size (.givenValues#)
	for .ival to size (.eigenvalues#)
		@assertApproximatelyEqual: .eigenvalues# [.ival], .givenValues# [.ival], 1e-13, "testEqualityOfEigenvalues"
	endfor
	appendInfoLine: " OK"	
endproc

procedure testEqualityOfEigenvectors: .eigen, .givenValues##
	appendInfo: tab$, tab$, tab$, "equality of eigenvectors:"
	selectObject: .eigen
	.numberOfEigenvalues = Get number of eigenvalues
	.dimension = Get eigenvector dimension
	assert numberOfRows (.givenValues##) = .numberOfEigenvalues
	assert numberOfColumns (.givenValues##) = .dimension
	for .ivec to .numberOfEigenvalues
		.evec# = Get eigenvector: .ivec
		.given# = row# (.givenValues##, .ivec)
		for .index to .dimension
			@assertApproximatelyEqual: .evec# [.index], .given# [.index], 1e-13, "testEqualityOfEigenvectors"
		endfor
	appendInfoLine: " OK"
endproc

procedure testOrthogonalityOfEigenvectors: .eigen
	appendInfo: tab$, tab$, tab$, "orhogonality of eigenvectors:"
	selectObject: .eigen
	.numberOfEigenvalues = Get number of eigenvalues
	for .ivec to .numberOfEigenvalues
		.eveci# = Get eigenvector: .ivec
		for .jvec from .ivec+1 to .numberOfEigenvalues
			.evecj# = Get eigenvector: .jvec
			.zero = inner (.eveci#, .evecj#)
			assert abs (.zero) < 1e-13 ; '.ivec' '.jvec' : '.zero'
		endfor
	endfor
	appendInfoLine: " OK"
endproc

procedure testDiagonals: .maxdim
	.text$ = tab$ + "test diagonals from 1 to " + string$ (.maxdim) + ":"
	appendInfoLine: .text$
	for .i to .maxdim
		@testDiagonal: .i
	endfor
	appendInfoLine: .text$ + " OK"
endproc

procedure diagonalData: .dim
	.name$ = string$(.dim) + "x" + string$ (.dim)
	.mat = Create simple Matrix: .name$, .dim, .dim, "0"
	.eigenvalues# = zero# (.dim)
	.eigenvectors## = zero## (.dim, .dim)
	for .i to .dim
		.val = .dim - .i + 1
		Set value: .i, .i, .val
		.eigenvalues# [.i] = .val
		.eigenvectors## [.i,.i] = 1
	endfor
endproc

procedure testDiagonal: .dim
	@diagonalData: .dim
	.matname$ = selected$ ("Matrix")
	.test$ = tab$ + tab$ + .matname$ + " diagonal:"
	appendInfoLine: .test$
	.eigen = To Eigen
	@testEqualityOfEigenvalues: .eigen, diagonalData.eigenvalues#
	@testEqualityOfEigenvectors: .eigen, diagonalData.eigenvectors##
	removeObject: .eigen, diagonalData.mat
	appendInfoLine: .test$, " OK"
endproc

procedure testGeneral_3by3
	.test$ = tab$ + "3x3 general "
	appendInfoLine: .test$
	.dim = 3
	.name$ = "3x3square"
	.mat## = {{0, 1, 0},
		...			{0, 0, 1},
		...			{1, 0, 0}}
	.mat = Create simple Matrix: .name$, .dim, .dim, ~ .mat## [row,col]
	# one of the eigenvalues is real (1) the other two are complex
	.givenValues_re# = {-1/2, -1/2, 1}
	.givenValues_im# = {0.5*sqrt(3), -0.5*sqrt(3), 0}
	.eigen = To Eigen (special): "general", .dim, "no"
	.eigenvalues_re# = List eigenvalues
	.eigenvalues_im# = List eigenvalues (imag)
	@testEqualityOfEigenvalues: .eigen, .givenValues_re#
	@testEqualityOfEigenvalues_im: .eigen, .givenValues_im#
	selectObject: .mat
	Eigen (complex)
	.eigenvectors = selected ("Matrix", 1)
	.eigenvalues = selected ("Matrix", 2)

	removeObject: .mat, .eigenvectors, .eigenvalues
	appendInfoLine: .test$, " OK"
endproc

procedure testWNKMatrices
	.test$ = "test W__n_(k) matrices"
	appendInfoLine: tab$, .test$
	for .n from 6 to 20
			@testEigenvaluesOfOneWNKMatrix: .n
	endfor
	appendInfoLine: tab$, .test$, " OK"
endproc

procedure testEigenvaluesOfOneWNKMatrix: .n
	# The special symmetric tridiagonal nxn matrices W__n_[k] are defined as
	# W__n_[k]: diagonal [i]      = k,                   i = 1,...,n
	#           offDiagonal [i]   = sqrt(i*(2*n-1-i)/4), i = 1,...,n-2
	#           offDiagonal [n-1] = sqrt (n*(n-1)/2)
	# The eigenvalues of W_n_(n+1) are {2*n, ..., 4, 2}.
	# The eigenvalues of the principal (n-1)x(n-1) submatrix (without last column and row)
	# are {2*n-1, ..., 5, 3}.
	# .n = 6
	.k = .n + 1
	appendInfo: tab$, tab$, "test W_" + string$(.n) + "(" + string$(.k) + "):"
	.wn = Create simple Matrix: "Wn", .n, .n, ~ 0.0
	.bnm1 = sqrt(0.5*.n*(.n-1))
	Formula: ~ if row = col then .k else 
	... if row = col+1 then if row < .n then 0.5*sqrt(col*(2*.n-1-col)) else .bnm1 fi else
	... if col = row+1 then if col < .n then 0.5*sqrt(row*(2*.n-1-row)) else .bnm1 fi else 
	... 0.0 fi fi fi
	.wneigen = To Eigen (special): "symmetric tridiagonal", .n, "no"
	for .ival to 	.n
		.eval = Get eigenvalue: .ival
		assert abs (.eval / (2 * (.n + 1 -.ival)) - 1.0) < 1.0e-7
	endfor
	# Create the principal submatrix
	.wnps = Create simple Matrix: "Wns", .n-1, .n-1, ~ object[.wn, row, col]
	.wnpseigen = To Eigen (special): "symmetric tridiagonal", .n-1, "no"
	for .ival to 	.n-1
		.eval = Get eigenvalue: .ival
		assert abs (.eval / (2 * (.n + 1 -.ival) - 1) - 1.0) < 1.0e-7
	endfor
	removeObject: .wn, .wneigen, .wnps, .wnpseigen
	appendInfoLine: " OK"
endproc

procedure testKacSylvesterMatrices
	.test$ = "test Kac-Sylvester(n) matrices"
	appendInfoLine: tab$, .test$
	for .n from 6 to 20
			@testEigenvaluesOfOneKacSylvesterMatrix: .n
	endfor
	appendInfoLine: tab$, .test$, " OK"
endproc

procedure testEigenvaluesOfOneKacSylvesterMatrix: .n
	# The (n+1)x(n+1) Kac-Sylvester matrix KS has
	# KS(n): diagonal[i]       = 0,     for i =1...n+1
	#        upperDiagonal [i] = n+1-i, for i = 1,...,n
	#        lowerDiagonal [i] = i,     for i = 1,...,n
	# The eigenvalues are {2*k-n} for k = 1,...,n
	# .n = 5
	appendInfo: tab$, tab$, "test KacSylvester (" + string$(.n) + "):"
	.ks = Create simple Matrix: "Kn", .n+1, .n+1, ~ 
	... if row = col+1 then col else
	... if col = row+1 then .n+1-row else 0 fi fi
	.eigen = To Eigen (special): "symmetric", .n, "no"
	for .ival to 	.n
		.eval = Get eigenvalue: .ival
		assert abs (.eval / (2 * .ival - .n) - 1.0) < 1.0e-7
	endfor
	appendInfoLine: " OK"	
endproc
label veryEnd
