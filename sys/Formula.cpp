/* Formula.cpp
 *
 * Copyright (C) 1992-2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#if defined (UNIX)
	#include <sys/stat.h>
#endif
#include "../dwsys/NUM2.h"
#include "Formula.h"
#include "Interpreter.h"
#include "Ui.h"
#include "praatP.h"
#include "praat_script.h"
#include "../kar/UnicodeData.h"
#include "../kar/longchar.h"
#include "UiPause.h"
#include "GuiTrust.h"
#include "Script.h"
#include "Notebook.h"
#include "DemoEditor.h"

static Interpreter theInterpreter;
static autoInterpreterStack theLocalInterpreterStack;
static Daata theSource;
static conststring32 theExpression;
static int theLevel = 1;
#define MAXIMUM_NUMBER_OF_LEVELS  20
static int theExpressionType [1 + MAXIMUM_NUMBER_OF_LEVELS];
static bool theOptimize;

typedef struct structFormulaInstruction {
	integer symbol;
	integer position;
	union {
		double number;
		integer label;
		char32 *string;
		Daata object;
		InterpreterVariable variable;
	} content;
} *FormulaInstruction;

static FormulaInstruction lexan, parse;
static integer ilabel, ilexan, iparse, numberOfInstructions, numberOfStringConstants;

enum { NO_SYMBOL_,

#define DECLARE_WITH_TENSORS(symbol)  \
	symbol, symbol##VEC_, symbol##MAT_,

/* First, all symbols after which "-" is unary. */
/* The list ends with "MINUS_" itself. */

	/* Opening symbols. */
	IF_, THEN_, ELSE_, OPENING_PARENTHESIS_, OPENING_BRACKET_, OPENING_BRACE_, COMMA_, COLON_, FROM_, TO_,
	/* Operators with boolean results. */
	OR_, AND_, NOT_, EQ_, NE_, LE_, LT_, GE_, GT_,
	/* Operators with results. */
	ADD_, SUB_, MUL_, RDIV_, IDIV_, MOD_, POWER_, CALL_, MINUS_,

/* Then, the symbols after which "-" is binary. */

	/* Closing symbols. */
	ENDIF_, FI_, CLOSING_PARENTHESIS_, CLOSING_BRACKET_, CLOSING_BRACE_,
	/* Things with a value. */
	#define LOW_VALUE  NUMBER_
		NUMBER_, NUMBER_PI_, NUMBER_E_, NUMBER_UNDEFINED_,
		/* Attributes of objects. */
		#define LOW_ATTRIBUTE  XMIN_
			XMIN_, XMAX_, YMIN_, YMAX_, NX_, NY_, DX_, DY_,
			ROW_, COL_, NROW_, NCOL_, ROW_STR_, COL_STR_, Y_, X_,
		#define HIGH_ATTRIBUTE  X_
	#define HIGH_VALUE  HIGH_ATTRIBUTE

	SELF_, SELF_STR_, OBJECT_, OBJECT_STR_, MATRIX_, MATRIX_STR_,
	STOPWATCH_,

/* The following symbols can be followed by "-" only if they are a variable. */

	/* Functions of 1 variable; if you add, update the #defines. */
	#define LOW_FUNCTION_1  ABS_
		DECLARE_WITH_TENSORS (ABS_)
		DECLARE_WITH_TENSORS (ROUND_)    DECLARE_WITH_TENSORS (FLOOR_)    DECLARE_WITH_TENSORS (CEILING_)
		DECLARE_WITH_TENSORS (RECTIFY_)
		DECLARE_WITH_TENSORS (SQRT_)
		DECLARE_WITH_TENSORS (SIN_)      DECLARE_WITH_TENSORS (COS_)      DECLARE_WITH_TENSORS (TAN_)
		DECLARE_WITH_TENSORS (ARCSIN_)   DECLARE_WITH_TENSORS (ARCCOS_)   DECLARE_WITH_TENSORS (ARCTAN_)
		SINC_, SINCPI_,
		DECLARE_WITH_TENSORS (EXP_)
		DECLARE_WITH_TENSORS (SINH_)     DECLARE_WITH_TENSORS (COSH_)     DECLARE_WITH_TENSORS (TANH_)
		DECLARE_WITH_TENSORS (ARCSINH_)  DECLARE_WITH_TENSORS (ARCCOSH_)  DECLARE_WITH_TENSORS (ARCTANH_)
		DECLARE_WITH_TENSORS (SIGMOID_)  DECLARE_WITH_TENSORS (INV_SIGMOID_)
		SOFTMAX_VEC_, SOFTMAX_PER_ROW_MAT_,
		ERF_, ERFC_, GAUSS_P_, GAUSS_Q_, INV_GAUSS_Q_,
		RANDOM_BERNOULLI_, RANDOM_BERNOULLI_VEC_,
		RANDOM_POISSON_, TRANSPOSE_MAT_,
		ROW_SUMS_VEC_, COLUMN_SUMS_VEC_,
		DECLARE_WITH_TENSORS (LOG2_)     DECLARE_WITH_TENSORS (LN_)       DECLARE_WITH_TENSORS (LOG10_)
		LN_GAMMA_,
		HERTZ_TO_BARK_, BARK_TO_HERTZ_, PHON_TO_DIFFERENCE_LIMENS_, DIFFERENCE_LIMENS_TO_PHON_,
		HERTZ_TO_MEL_, MEL_TO_HERTZ_, HERTZ_TO_SEMITONES_, SEMITONES_TO_HERTZ_,
		ERB_, HERTZ_TO_ERB_, ERB_TO_HERTZ_,
		SUM_, MEAN_, STDEV_, CENTER_, RANDOM_IMAX_,
		EVALUATE_, EVALUATE_NOCHECK_, EVALUATE_STR_, EVALUATE_NOCHECK_STR_,
		STRING_STR_, VERTICAL_STR_, NUMBERS_VEC_, SLEEP_, UNICODE_, UNICODE_STR_,
	#define HIGH_FUNCTION_1  UNICODE_STR_

	/* Functions of 2 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_2  ARCTAN2_
		ARCTAN2_, RANDOM_UNIFORM_, RANDOM_INTEGER_, RANDOM_GAUSS_, RANDOM_BINOMIAL_,
		RANDOM_GAMMA_,
		CHI_SQUARE_P_, CHI_SQUARE_Q_, INCOMPLETE_GAMMAP_,
		INV_CHI_SQUARE_Q_, STUDENT_P_, STUDENT_Q_, INV_STUDENT_Q_,
		BETA_, BETA2_, BESSEL_I_, BESSEL_K_, LN_BETA_,
		SOUND_PRESSURE_TO_PHON_, OBJECTS_ARE_IDENTICAL_,
		ROW_VEC_, COL_VEC_,
		INNER_, CORRELATION_, OUTER_MAT_, MUL_VEC_, MUL_MAT_, MUL_FAST_MAT_, MUL_METAL_MAT_,
		MUL_TN_MAT_, MUL_NT_MAT_, MUL_TT_MAT_, REPEAT_VEC_,
		ROW_INNERS_VEC_, SOLVE_VEC_, SOLVE_MAT_,
	#define HIGH_FUNCTION_2  SOLVE_MAT_

	/* Functions of 3 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_3  FISHER_P_
		FISHER_P_, FISHER_Q_, INV_FISHER_Q_,
		BINOMIAL_P_, BINOMIAL_Q_, INCOMPLETE_BETA_, INV_BINOMIAL_P_, INV_BINOMIAL_Q_,
	#define HIGH_FUNCTION_3  INV_BINOMIAL_Q_

	/* Functions of 4 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_4 SOLVE_WEAKLYCONSTRAINED_VEC_
		SOLVE_WEAKLYCONSTRAINED_VEC_,
	#define HIGH_FUNCTION_4 SOLVE_WEAKLYCONSTRAINED_VEC_

	/* Functions of a variable number of variables; if you add, update the #defines. */
	#define LOW_FUNCTION_N  DO_
		DO_, DOSTR_,
		WRITE_INFO_, WRITE_INFO_LINE_, APPEND_INFO_, APPEND_INFO_LINE_,
		WRITE_FILE_, WRITE_FILE_LINE_, APPEND_FILE_, APPEND_FILE_LINE_,
		PAUSE_SCRIPT_, EXIT_SCRIPT_, RUN_SCRIPT_, RUN_SCRIPT_WITH_FORM_, RUN_NOTEBOOK_,
		RUN_SYSTEM_, RUN_SYSTEM_STR_, RUN_SYSTEM_NOCHECK_, RUN_SUBPROCESS_, RUN_SUBPROCESS_STR_,
		MIN_, MIN_E_, MIN_IGNORE_UNDEFINED_,
		MAX_, MAX_E_, MAX_IGNORE_UNDEFINED_,
		IMIN_, IMIN_E_, IMIN_IGNORE_UNDEFINED_,
		IMAX_, IMAX_E_, IMAX_IGNORE_UNDEFINED_,
		QUANTILE_,
		NORM_,
		LEFT_STR_, RIGHT_STR_, MID_STR_,
		PAD_LEFT_STR_, PAD_RIGHT_STR_, TRUNCATE_LEFT_STR_, TRUNCATE_RIGHT_STR_, PAD_OR_TRUNCATE_LEFT_STR_, PAD_OR_TRUNCATE_RIGHT_STR_,
		SELECTED_, SELECTED_STR_, NUMBER_OF_SELECTED_, SELECTED_VEC_, SELECTED_STRVEC_,
		SELECT_OBJECT_, PLUS_OBJECT_, MINUS_OBJECT_, REMOVE_OBJECT_,
		ASK_FOR_TRUST_, BEGIN_PAUSE_,
		REAL_, POSITIVE_, INTEGER_, NATURAL_,
		WORD_, SENTENCE_, TEXT_, BOOLEAN_,
		CHOICE_, OPTIONMENU_, OPTION_MENU_, OPTION_,
		INFILE_, OUTFILE_, FOLDER_,
		REALVECTOR_, POSITIVEVECTOR_, INTEGERVECTOR_, NATURALVECTOR_,
		HEADING_, COMMENT_, END_PAUSE_,
		LOWER_CASE_APP_NAME_STR_, UPPER_CASE_APP_NAME_STR_,
		CLOCK_,
		APP_VERSION_, APP_VERSION_STR_, APP_YEAR_, APP_MONTH_, APP_MONTH_STR_, APP_DAY_,
		CHOOSE_READ_FILE_STR_, CHOOSE_WRITE_FILE_STR_, CHOOSE_FOLDER_STR_, CHOOSE_DIRECTORY_STR_,
		DEMO_WINDOW_TITLE_, DEMO_SHOW_, DEMO_WAIT_FOR_INPUT_, DEMO_PEEK_INPUT_, DEMO_INPUT_, DEMO_CLICKED_IN_,
		DEMO_CLICKED_, DEMO_X_, DEMO_Y_, DEMO_KEY_PRESSED_, DEMO_KEY_,
		DEMO_SHIFT_KEY_PRESSED_, DEMO_COMMAND_KEY_PRESSED_, DEMO_OPTION_KEY_PRESSED_,
		ZERO_VEC_, ZERO_MAT_,
		LINEAR_VEC_, LINEAR_MAT_, TO_VEC_, FROM_TO_VEC_, FROM_TO_BY_VEC_, FROM_TO_COUNT_VEC_, BETWEEN_BY_VEC_, BETWEEN_COUNT_VEC_,
		SORT_VEC_, SORT_REMOVE_UNDEFINED_VEC_, SORT_STRVEC_, SORT_NUMBER_AWARE_STRVEC_, SHUFFLE_VEC_, SHUFFLE_STRVEC_,
		RANDOM_UNIFORM_VEC_, RANDOM_UNIFORM_MAT_,
		RANDOM_INTEGER_VEC_, RANDOM_INTEGER_MAT_,
		RANDOM_GAUSS_VEC_, RANDOM_GAUSS_MAT_,
		RANDOM_GAMMA_VEC_, RANDOM_GAMMA_MAT_,
		SOLVE_SPARSE_VEC_, SOLVE_NONNEGATIVE_VEC_,
		PEAKS_MAT_,
		SIZE_, NUMBER_OF_ROWS_, NUMBER_OF_COLUMNS_, COMBINE_VEC_, PART_VEC_, PART_MAT_, EDITOR_,
		RANDOM__INITIALIZE_WITH_SEED_UNSAFELY_BUT_PREDICTABLY_, RANDOM__INITIALIZE_SAFELY_AND_UNPREDICTABLY_,
		HASH_, HEX_STR_, UNHEX_STR_,
		EMPTY_STRVEC_, READ_LINES_FROM_FILE_STRVEC_,
		FILE_NAMES_STRVEC_, FOLDER_NAMES_STRVEC_, FILE_NAMES_CASE_INSENSITIVE_STRVEC_, FOLDER_NAMES_CASE_INSENSITIVE_STRVEC_,
		SPLIT_BY_WHITESPACE_STRVEC_, SPLIT_BY_STRVEC_,
		LOWER_CASE_STR_, LOWER_CAMEL_CASE_STR_, LOWER_SNAKE_CASE_STR_,
		UPPER_CASE_STR_, UPPER_CAMEL_CASE_STR_, UPPER_SNAKE_CASE_STR_,
	#define HIGH_FUNCTION_N  UPPER_SNAKE_CASE_STR_

	/* String functions. */
	#define LOW_STRING_FUNCTION  LOW_FUNCTION_STR1
		#define LOW_FUNCTION_STR1  LENGTH_
			LENGTH_, STRING_TO_NUMBER_, FILE_READABLE_, FOLDER_EXISTS_, TRY_TO_WRITE_FILE_, TRY_TO_APPEND_FILE_, DELETE_FILE_,
			CREATE_FOLDER_, CREATE_DIRECTORY_, SET_WORKING_DIRECTORY_, VARIABLE_EXISTS_,
			READ_FILE_, READ_FILE_STR_, READ_FILE_VEC_, READ_FILE_MAT_,
			UNICODE_TO_BACKSLASH_TRIGRAPHS_STR_, BACKSLASH_TRIGRAPHS_TO_UNICODE_STR_, ENVIRONMENT_STR_,
		#define HIGH_FUNCTION_STR1  ENVIRONMENT_STR_
		#define LOW_FUNCTION_STR0  DATE_STR_
			DATE_STR_, DATE_UTC_STR_, DATE_ISO_STR_, DATE_UTC_ISO_STR_, DATE_VEC_, DATE_UTC_VEC_, INFO_STR_,   // TODO: two of those aren't really string functions
		#define HIGH_FUNCTION_STR0  INFO_STR_
		#define LOW_FUNCTION_STR2  INDEX_
			INDEX_, INDEX_CASE_INSENSITIVE_, RINDEX_, RINDEX_CASE_INSENSITIVE_,
			STARTS_WITH_, STARTS_WITH_CASE_INSENSITIVE_, ENDS_WITH_, ENDS_WITH_CASE_INSENSITIVE_,
			INDEX_REGEX_, RINDEX_REGEX_, EXTRACT_NUMBER_,
			MOVE_AND_OR_RENAME_FILE_,
		#define HIGH_FUNCTION_STR2  MOVE_AND_OR_RENAME_FILE_
		EXTRACT_WORD_STR_, EXTRACT_LINE_STR_,
		REPLACE_STR_, REPLACE_REGEX_STR_,
		FIXED_STR_, PERCENT_STR_, HEXADECIMAL_STR_,
	#define HIGH_STRING_FUNCTION  HEXADECIMAL_STR_

	/* Range functions. */
	#define LOW_RANGE_FUNCTION  SUM_OVER_
		SUM_OVER_,
	#define HIGH_RANGE_FUNCTION  SUM_OVER_

	#define LOW_FUNCTION  LOW_FUNCTION_1
	#define HIGH_FUNCTION  HIGH_RANGE_FUNCTION

	/* Membership operator. */
	PERIOD_,
	#define highestInputSymbol  PERIOD_

/* Symbols introduced by the parser. */

	TRUE_, FALSE_,
	GOTO_, IFTRUE_, IFFALSE_, INCREMENT_GREATER_GOTO_,
	LABEL_,
	DECREMENT_AND_ASSIGN_, ADD_3DOWN_, POP_2_,
	VEC_CELL_, MAT_CELL_, STRVEC_CELL_, VARIABLE_REFERENCE_,
	TENSOR_LITERAL_, TENSOR_LITERAL_CELL_,
	SELF0_, SELFSTR0_, TO_OBJECT_,
	OBJECT_XMIN_, OBJECT_XMAX_, OBJECT_YMIN_, OBJECT_YMAX_, OBJECT_NX_, OBJECT_NY_,
	OBJECT_DX_, OBJECT_DY_, OBJECT_NROW_, OBJECT_NCOL_, OBJECT_ROW_STR_, OBJECT_COL_STR_,
	OBJECTCELL0_, OBJECTCELL0_STR_, OBJECTCELL1_, OBJECTCELL1_STR_, OBJECTCELL2_, OBJECTCELL2_STR_,
	OBJECTLOCATION0_, OBJECTLOCATION0_STR_, OBJECTLOCATION1_, OBJECTLOCATION1_STR_, OBJECTLOCATION2_, OBJECTLOCATION2_STR_,
	SELFMATRIX1_, SELFMATRIX1_STR_, SELFMATRIX2_, SELFMATRIX2_STR_,
	SELFFUNCTION1_, SELFFUNCTION1_STR_, SELFFUNCTION2_, SELFFUNCTION2_STR_,
	MATRIX0_, MATRIX0_STR_, MATRIX1_, MATRIX1_STR_, MATRIX2_, MATRIX2_STR_,
	FUNCTION0_, FUNCTION0_STR_, FUNCTION1_, FUNCTION1_STR_, FUNCTION2_, FUNCTION2_STR_,
	SQR_,

/* Symbols introduced by lexical analysis. */

	STRING_,
	NUMERIC_VARIABLE_, NUMERIC_VECTOR_VARIABLE_, NUMERIC_MATRIX_VARIABLE_,
	STRING_VARIABLE_, STRING_ARRAY_VARIABLE_,
	VARIABLE_NAME_, INDEXED_NUMERIC_VARIABLE_, INDEXED_STRING_VARIABLE_,
	END_
	#define highestSymbol END_
};

/* The names that start with an underscore (_) do not occur in the formula text: */
/* they are used in error messages and in debugging (see Formula_print). */

static const conststring32 Formula_instructionNames [1 + highestSymbol] = { U"",
	#define NAME_WITH_TENSORS(name)  \
		U"" #name, U"" #name "#", U"" #name "##",

	U"if", U"then", U"else", U"(", U"[", U"{", U",", U":", U"from", U"to",
	U"or", U"and", U"not", U"=", U"<>", U"<=", U"<", U">=", U">",
	U"+", U"-", U"*", U"/", U"div", U"mod", U"^", U"_call", U"_neg",
	U"endif", U"fi", U")", U"]", U"}",
	U"a number", U"pi", U"e", U"undefined",
	U"xmin", U"xmax", U"ymin", U"ymax", U"nx", U"ny", U"dx", U"dy",
	U"row", U"col", U"nrow", U"ncol", U"row$", U"col$", U"y", U"x",
	U"self", U"self$", U"object", U"object$", U"_matrix", U"_matrix$",
	U"stopwatch",
	NAME_WITH_TENSORS (abs)
	NAME_WITH_TENSORS (round)    NAME_WITH_TENSORS (floor)    NAME_WITH_TENSORS (ceiling)
	NAME_WITH_TENSORS (rectify)
	NAME_WITH_TENSORS (sqrt)
	NAME_WITH_TENSORS (sin)      NAME_WITH_TENSORS (cos)      NAME_WITH_TENSORS (tan)
	NAME_WITH_TENSORS (arcsin)   NAME_WITH_TENSORS (arccos)   NAME_WITH_TENSORS (arctan)
	U"sinc", U"sincpi",
	NAME_WITH_TENSORS (exp)
	NAME_WITH_TENSORS (sinh)     NAME_WITH_TENSORS (cosh)     NAME_WITH_TENSORS (tanh)
	NAME_WITH_TENSORS (arcsinh)  NAME_WITH_TENSORS (arccosh)  NAME_WITH_TENSORS (arctanh)
	NAME_WITH_TENSORS (sigmoid)  NAME_WITH_TENSORS (invSigmoid)
	U"softmax#", U"softmaxPerRow##",
	U"erf", U"erfc", U"gaussP", U"gaussQ", U"invGaussQ",
	U"randomBernoulli", U"randomBernoulli#",
	U"randomPoisson", U"transpose##",
	U"rowSums#", U"columnSums#",
	NAME_WITH_TENSORS (log2)     NAME_WITH_TENSORS (ln)       NAME_WITH_TENSORS (log10)
	U"lnGamma",
	U"hertzToBark", U"barkToHertz", U"phonToDifferenceLimens", U"differenceLimensToPhon",
	U"hertzToMel", U"melToHertz", U"hertzToSemitones", U"semitonesToHertz",
	U"erb", U"hertzToErb", U"erbToHertz",
	U"sum", U"mean", U"stdev", U"center", U"randomImax",
	U"evaluate", U"evaluate_nocheck", U"evaluate$", U"evaluate_nocheck$",
	U"string$", U"vertical$", U"numbers#", U"sleep", U"unicode", U"unicode$",
	U"arctan2", U"randomUniform", U"randomInteger", U"randomGauss", U"randomBinomial",
	U"randomGamma",
	U"chiSquareP", U"chiSquareQ", U"incompleteGammaP", U"invChiSquareQ", U"studentP", U"studentQ", U"invStudentQ",
	U"beta", U"beta2", U"besselI", U"besselK", U"lnBeta",
	U"soundPressureToPhon", U"objectsAreIdentical",
	U"row#", U"col#",
	U"inner", U"correlation", U"outer##", U"mul#", U"mul##", U"mul_fast##", U"mul_metal##",
	U"mul_tn##", U"mul_nt##", U"mul_tt##", U"repeat#",
	U"rowInners#", U"solve#", U"solve##",
	U"fisherP", U"fisherQ", U"invFisherQ",
	U"binomialP", U"binomialQ", U"incompleteBeta", U"invBinomialP", U"invBinomialQ",
	U"solveWeaklyConstrained#",
	U"do", U"do$",
	U"writeInfo", U"writeInfoLine", U"appendInfo", U"appendInfoLine",
	U"writeFile", U"writeFileLine", U"appendFile", U"appendFileLine",
	U"pauseScript", U"exitScript", U"runScript", U"runScriptWithForm", U"runNotebook",
	U"runSystem", U"runSystem$", U"runSystem_nocheck", U"runSubprocess", U"runSubprocess$",
	U"min", U"min_e", U"min_removeUndefined",
	U"max", U"max_e", U"max_removeUndefined",
	U"imin", U"imin_e", U"imin_removeUndefined",
	U"imax", U"imax_e", U"imax_removeUndefined",
	U"quantile",
	U"norm",
	U"left$", U"right$", U"mid$",
	U"padLeft$", U"padRight$", U"truncateLeft$", U"truncateRight$", U"padOrTruncateLeft$", U"padOrTruncateRight$",
	U"selected", U"selected$", U"numberOfSelected", U"selected#", U"selected$#",
	U"selectObject", U"plusObject", U"minusObject", U"removeObject",
	U"askForTrust", U"beginPause",
	U"real", U"positive", U"integer", U"natural",
	U"word", U"sentence", U"text", U"boolean",
	U"choice", U"optionmenu", U"optionMenu", U"option",
	U"infile", U"outfile", U"folder",
	U"realvector", U"positivevector", U"integervector", U"naturalvector",
	U"heading", U"comment", U"endPause",
	U"lowerCaseAppName$", U"upperCaseAppName$",
	U"clock",
	U"appVersion", U"appVersion$", U"appYear", U"appMonth", U"appMonth$", U"appDay",
	U"chooseReadFile$", U"chooseWriteFile$", U"chooseFolder$", U"chooseDirectory$",
	U"demoWindowTitle", U"demoShow", U"demoWaitForInput", U"demoPeekInput", U"demoInput", U"demoClickedIn",
	U"demoClicked", U"demoX", U"demoY", U"demoKeyPressed", U"demoKey$",
	U"demoShiftKeyPressed", U"demoCommandKeyPressed", U"demoOptionKeyPressed",
	U"zero#", U"zero##",
	U"linear#", U"linear##", U"to#", U"from_to#", U"from_to_by#", U"from_to_count#", U"between_by#", U"between_count#",
	U"sort#", U"sort_removeUndefined#", U"sort$#", U"sort_numberAware$#", U"shuffle#", U"shuffle$#",
	U"randomUniform#", U"randomUniform##",
	U"randomInteger#", U"randomInteger##",
	U"randomGauss#", U"randomGauss##",
	U"randomGamma#", U"randomGamma##", U"solveSparse#", U"solveNonnegative#",
	U"peaks##",
	U"size", U"numberOfRows", U"numberOfColumns", U"combine#", U"part#", U"part##", U"editor",
	U"random_initializeWithSeedUnsafelyButPredictably", U"random_initializeSafelyAndUnpredictably",
	U"hash", U"hex$", U"unhex$",
	U"empty$#", U"readLinesFromFile$#",
	U"fileNames$#", U"folderNames$#", U"fileNames_caseInsensitive$#", U"folderNames_caseInsensitive$#",
	U"splitByWhitespace$#", U"splitBy$#",
	U"lowerCase$", U"lowerCamelCase$", U"lowerSnakeCase$",
	U"upperCase$", U"upperCamelCase$", U"upperSnakeCase$",

	// LOW_FUNCTION_STR1
		U"length", U"number", U"fileReadable", U"folderExists", U"tryToWriteFile", U"tryToAppendFile", U"deleteFile",
		U"createFolder", U"createDirectory", U"setWorkingDirectory", U"variableExists",
		U"readFile", U"readFile$", U"readFile#", U"readFile##",
		U"unicodeToBackslashTrigraphs$", U"backslashTrigraphsToUnicode$", U"environment$",
	// HIGH_FUNCTION_STR1
	U"date$", U"date_utc$", U"date_iso$", U"date_utc_iso$", U"date#", U"date_utc#", U"info$",
	// LOW_FUNCTION_STR2
		U"index", U"index_caseInsensitive", U"rindex", U"rindex_caseInsensitive",
		U"startsWith", U"startsWith_caseInsensitive", U"endsWith", U"endsWith_caseInsensitive",
		U"index_regex", U"rindex_regex", U"extractNumber",
		U"moveAndOrRenameFile",
	// HIGH_FUNCTION_STR2
	U"extractWord$", U"extractLine$",
	U"replace$", U"replace_regex$",
	U"fixed$", U"percent$", U"hexadecimal$",
	U"sumOver",
	U".",
	U"_true", U"_false",
	U"_goto", U"_iftrue", U"_iffalse", U"_incrementGreaterGoto",
	U"_label",
	U"_decrementAndAssign", U"_add3Down", U"_pop2",
	U"_numericVectorElement", U"_numericMatrixElement", U"_stringVectorElement", U"_variableReference",
	U"_tensorLiteral", U"_tensorLiteralCell",
	U"_self0", U"_self0$", U"_toObject",
	U"_object_xmin", U"_object_xmax", U"_object_ymin", U"_object_ymax", U"_object_nx", U"_object_ny",
	U"_object_dx", U"_object_dy", U"_object_nrow", U"_object_ncol", U"_object_row$", U"_object_col$",
	U"_objectcell0", U"_objectcell0$", U"_objectcell1", U"_objectcell1$", U"_objectcell2", U"_objectcell2$",
	U"_objectlocation0", U"_objectlocation0$", U"_objectlocation1", U"_objectlocation1$", U"_objectlocation2", U"_objectlocation2$",
	U"_selfmatrix1", U"_selfmatrix1$", U"_selfmatrix2", U"_selfmatrix2$",
	U"_selffunction1", U"_selffunction1$", U"_selffunction2", U"_selffunction2$",
	U"_matrix0", U"_matrix0$", U"_matrix1", U"_matrix1$", U"_matrix2", U"_matrix2$",
	U"_function0", U"_function0$", U"_function1", U"_function1$", U"_function2", U"_function2$",
	U"_square",
	U"_string",
	U"a numeric variable", U"a vector variable", U"a matrix variable",
	U"a string variable", U"a string array variable",
	U"a variable name", U"an indexed numeric variable", U"an indexed string variable",
	U"the end of the formula"
};

#define newlabel (-- ilabel)
#define newread (lexan [++ ilexan]. symbol)
#define oldread  (-- ilexan)

static void formulaError (conststring32 message, integer position) {
	static MelderString truncatedExpression;
	MelderString_ncopy (& truncatedExpression, theExpression, position + 1);
	Melder_throw (message, U":\n« ", truncatedExpression.string);
}

static integer Formula_hasLanguageName (conststring32 f) {
	static autoINTVEC index;
	if (NUMisEmpty (index.get())) {
		index = to_INTVEC (highestInputSymbol);
		std::sort (index.begin(), index.end(),
			[] (integer i, integer j) {
				return str32cmp (Formula_instructionNames [i], Formula_instructionNames [j]) < 0;
			}
		);
	}
	constexpr integer sentinel = 0;   // has to be different from the numbers 1 .. index.size
	const integer *const found = std::lower_bound (index.begin(), index.end(), sentinel,
		[f] (integer i, integer j) {
			return str32cmp (
				i == sentinel ? f : Formula_instructionNames [i],
				j == sentinel ? f : Formula_instructionNames [j]
			) < 0;
		}
	);
	if (found != index.end() && Melder_equ (Formula_instructionNames [*found], f))
		return *found;
	return 0;
}

static void Formula_lexan () {
/*
	Purpose:
		translate the formula text into a series of symbols.
	Postcondition:
		if not thrown, then the last symbol is END_.
	Example:
		the text "x*7" yields 5 symbols:
			lexan [0] is empty;
			lexan [1]. symbol = X_;
			lexan [2]. symbol = MUL_;
			lexan [3]. symbol = NUMBER_;
			lexan [3]. number = 7.00000000e+00;
			lexan [4]. symbol = END_;
*/
	char32 kar;   // the character most recently read from theExpression
	integer ikar = -1;   // the position of that character in theExpression
#define newchar kar = theExpression [++ ikar]
#define oldchar -- ikar

	integer itok = 0;   // position of most recent symbol in "lexan"
#define newtok(s)  { lexan [++ itok]. symbol = s; lexan [itok]. position = ikar; }
#define toknumber(g)  lexan [itok]. content.number = (g)
#define tokmatrix(m)  lexan [itok]. content.object = (m)

	static MelderString token;   // string to collect a symbol name in
#define stringtokon MelderString_empty (& token);
#define stringtokchar { MelderString_appendCharacter (& token, kar); newchar; }
#define stringtokoff (void) 0

	ilexan = iparse = ilabel = numberOfStringConstants = 0;
	do {
		newchar;
		if (Melder_isHorizontalOrVerticalSpace (kar)) {
			;   // ignore spaces and tabs
		} else if (kar == U'\0') {
			newtok (END_)
		} else if (Melder_isAsciiDecimalNumber (kar)) {
			char32 saveKar = kar;
			bool isHexadecimal = false;
			if (kar == U'0') {
				newchar;
				if (kar == U'x') {
					isHexadecimal = true;
					newchar;
				} else {
					oldchar;
				}
			}
			if (isHexadecimal) {
				stringtokon;
				do stringtokchar while (Melder_isHexadecimalDigit (kar));
				stringtokoff;
				oldchar;
				newtok (NUMBER_)
				toknumber (strtoull (Melder_peek32to8 (token.string), nullptr, 16));
			} else {
				kar = saveKar;
				stringtokon;
				do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				if (kar == U'.')
					do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				if (kar == U'e' || kar == U'E') {
					kar = U'e';
					stringtokchar
					if (kar == U'-')
						stringtokchar
					else if (kar == U'+')
						newchar;
					if (! Melder_isAsciiDecimalNumber (kar))
						formulaError (U"Missing exponent", ikar);
					do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				}
				if (kar == U'%')
					stringtokchar
				stringtokoff;
				oldchar;
				newtok (NUMBER_)
				toknumber (Melder_atof (token.string));
			}
		} else if (Melder_isLetter (kar) && ! Melder_isUpperCaseLetter (kar) ||
				(kar == U'.' && Melder_isLetter (theExpression [ikar + 1]) && ! Melder_isUpperCaseLetter (theExpression [ikar + 1])
				&& (itok == 0 || (lexan [itok]. symbol != MATRIX_ && lexan [itok]. symbol != MATRIX_STR_
					&& lexan [itok]. symbol != CLOSING_BRACKET_)))) {
			integer tok;
			bool isString = false;
			int rank = 0;
			stringtokon;
			do stringtokchar while (Melder_isWordCharacter (kar) || kar == U'.');
			if (kar == U'$') {
				isString = true;
				stringtokchar
				if (kar == U'#') {
					rank += 1;
					stringtokchar
				}
			}
			if (kar == U'#') {
				rank += 1;
				stringtokchar
				if (kar == U'#') {
					rank += 1;
					stringtokchar
					if (kar == U'#') {
						rank += 1;
						stringtokchar
						if (kar == U'#') {
							rank += 1;
							stringtokchar
						}
					}
				}
			}
			stringtokoff;
			oldchar;
			/*
				`token` now contains a word, possibly ending in $, #, ## or $#;
				it could be a variable name, a function name, both, or a procedure name.
				Try a language or function name first.
			*/
			tok = Formula_hasLanguageName (token.string);
			if (tok) {
				/*
					We have a language name or a function name. It MIGHT be meant to be a variable name, though,
					regarding the large and expanding number of language names.
				*/
				/*
					First the constants. They are reserved words and can never be variable names.
				*/
				if (tok == NUMBER_PI_) {
					newtok (NUMBER_)
					toknumber (NUMpi);
				} else if (tok == NUMBER_E_) {
					newtok (NUMBER_)
					toknumber (NUMe);
				} else if (tok == NUMBER_UNDEFINED_) {
					newtok (NUMBER_)
					toknumber (undefined);
				/*
					One very common language name must be converted to a synonym.
				*/
				} else if (tok == FI_) {
					newtok (ENDIF_)
				/*
					Is it a function name? These may be ambiguous.
				*/
				} else if (tok >= LOW_FUNCTION && tok <= HIGH_FUNCTION) {
					/*
						Look ahead to find out whether the next token is a left parenthesis (or a colon).
					*/
					integer jkar;
					jkar = ikar + 1;
					while (Melder_isHorizontalSpace (theExpression [jkar]))
						jkar ++;
					if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
						newtok (tok)   // this must be a function name
					} else if (theExpression [jkar] == U'[' && rank == 0) {
						if (isString) {
							newtok (INDEXED_STRING_VARIABLE_)
						} else {
							newtok (INDEXED_NUMERIC_VARIABLE_)
						}
						lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
						numberOfStringConstants ++;
					} else {
						/*
							This could be a variable with the same name as a function.
						*/
						const InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
						if (! var) {
							newtok (VARIABLE_NAME_)
							lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
							numberOfStringConstants ++;
						} else {
							if (rank == 0) {
								if (isString) {
									newtok (STRING_VARIABLE_)
								} else {
									newtok (NUMERIC_VARIABLE_)
								}
							} else if (rank == 1) {
								if (isString) {
									newtok (STRING_ARRAY_VARIABLE_)
								} else {
									newtok (NUMERIC_VECTOR_VARIABLE_)
								}
							} else if (rank == 2) {
								if (isString) {
									formulaError (U"String matrices not implemented.", ikar);
								} else {
									newtok (NUMERIC_MATRIX_VARIABLE_)
								}
							} else if (rank == 3) {
								formulaError (U"Rank-3 tensors not implemented.", ikar);
							} else {
								formulaError (U"Rank-4 tensors not implemented.", ikar);
							}
							lexan [itok]. content.variable = var;
						}
					}
				/*
					Not a function name.
					Must be a language name (if, then, else, endif, or, and, not, div, mod, x, ncol, stopwatch).
					Some can be used as variable names (x, ncol...).
				*/
				} else if (tok >= LOW_ATTRIBUTE && tok <= HIGH_ATTRIBUTE) {
					/*
						Look back to find out whether this is an attribute.
					*/
					if (itok > 0 && lexan [itok]. symbol == PERIOD_) {
						/*
							This must be an attribute that follows a period.
						*/
						newtok (tok)
					} else if (theSource) {
						/*
							Look for ambiguity.
						*/
						if (Interpreter_hasVariable (theInterpreter, token.string))
							Melder_throw (
								U"«", token.string,
								U"» is ambiguous: a variable or an attribute of the current object. "
								U"Please change variable name.");
						if (tok == ROW_ || tok == COL_ || tok == X_ || tok == Y_) {
							newtok (tok)
						} else {
							newtok (MATRIX_)
							tokmatrix (theSource);
							newtok (PERIOD_)
							newtok (tok)
						}
					} else {
						/*
							This must be a variable, since there is no "current object" here.
						*/
						integer jkar = ikar + 1;
						while (Melder_isHorizontalSpace (theExpression [jkar])) jkar ++;
						if (theExpression [jkar] == U'[' && rank == 0) {
							if (isString) {
								newtok (INDEXED_STRING_VARIABLE_)
							} else {
								newtok (INDEXED_NUMERIC_VARIABLE_)
							}
							lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
							numberOfStringConstants ++;
						} else {
							const InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
							if (! var) {
								newtok (VARIABLE_NAME_)
								lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
								numberOfStringConstants ++;
							} else {
								if (rank == 0) {
									if (isString) {
										newtok (STRING_VARIABLE_)
									} else {
										newtok (NUMERIC_VARIABLE_)
									}
								} else if (rank == 1) {
									if (isString) {
										newtok (STRING_ARRAY_VARIABLE_)
									} else {
										newtok (NUMERIC_VECTOR_VARIABLE_)
									}
								} else if (rank == 2) {
									if (isString) {
										formulaError (U"String matrices not implemented.", ikar);
									} else {
										newtok (NUMERIC_MATRIX_VARIABLE_)
									}
								} else if (rank == 3) {
									formulaError (U"Rank-3 tensors not implemented.", ikar);
								} else {
									formulaError (U"Rank-4 tensors not implemented.", ikar);
								}
								lexan [itok]. content.variable = var;
							}
						}
					}
				} else {
					newtok (tok)   // this must be a language name
				}
			} else {
				/*
					token.string is not a language name
				*/
				integer jkar = ikar + 1;
				while (Melder_isHorizontalSpace (theExpression [jkar]))
					jkar ++;
				if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
					Melder_throw (
						U"Unknown function «", token.string, U"» in formula.");
				} else if (theExpression [jkar] == '[' && rank == 0) {
					if (isString) {
						newtok (INDEXED_STRING_VARIABLE_)
					} else {
						newtok (INDEXED_NUMERIC_VARIABLE_)
					}
					lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
					numberOfStringConstants ++;
				} else {
					InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
					if (! var) {
						newtok (VARIABLE_NAME_)
						lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
						numberOfStringConstants ++;
					} else {
						if (rank == 0) {
							if (isString) {
								newtok (STRING_VARIABLE_)
							} else {
								newtok (NUMERIC_VARIABLE_)
							}
						} else if (rank == 1) {
							if (isString) {
								newtok (STRING_ARRAY_VARIABLE_)
							} else {
								newtok (NUMERIC_VECTOR_VARIABLE_)
							}
						} else if (rank == 2) {
							if (isString) {
								formulaError (U"String matrices not implemented.", ikar);
							} else {
								newtok (NUMERIC_MATRIX_VARIABLE_)
							}
						} else if (rank == 3) {
							formulaError (U"Rank-3 tensors not implemented.", ikar);
						} else {
							formulaError (U"Rank-4 tensors not implemented.", ikar);
						}
						lexan [itok]. content.variable = var;
					}
				}
			}
		} else if (kar >= U'A' && kar <= U'Z') {
			bool endsInDollarSign = false;
			stringtokon;
			do stringtokchar while (Melder_isAlphanumeric (kar) || kar == U'_');
			if (kar == U'$') {
				stringtokchar
				endsInDollarSign = true;
			}
			stringtokoff;
			oldchar;
			/*
				`token` now contains a word that could be an object name.
			*/
			char32 *underscore = str32chr (token.string, '_');
			if (str32equ (token.string, U"Self")) {
				if (! theSource)
					formulaError (U"Cannot use “Self” if there is no current object.", ikar);
				newtok (MATRIX_)
				tokmatrix (theSource);
			} else if (str32equ (token.string, U"Self$")) {
				if (! theSource)
					formulaError (U"Cannot use “Self$” if there is no current object.", ikar);
				newtok (MATRIX_STR_)
				tokmatrix (theSource);
			} else if (! underscore) {
				Melder_throw (
					U"Unknown symbol «", token.string, U"» in formula "
					U"(variables start with nonupper case; object names contain an underscore).");
			} else if (str32nequ (token.string, U"Object_", 7)) {
				const integer uniqueID = Melder_atoi (token.string + 7);
				integer i = theCurrentPraatObjects -> n;
				while (i > 0 && uniqueID != theCurrentPraatObjects -> list [i]. id)
					i --;
				if (i == 0)
					formulaError (U"No such object (note: variables start with nonupper case)", ikar);
				newtok (endsInDollarSign ? MATRIX_STR_ : MATRIX_)
				tokmatrix ((Daata) theCurrentPraatObjects -> list [i]. object);
			} else {
				integer i = theCurrentPraatObjects -> n;
				*underscore = U' ';
				if (endsInDollarSign)
					token.string [-- token.length] = U'\0';
				while (i > 0 && ! str32equ (token.string, theCurrentPraatObjects -> list [i]. name.get()))
					i --;
				if (i == 0)
					formulaError (U"No such object (note: variables start with nonupper case)", ikar);
				newtok (endsInDollarSign ? MATRIX_STR_ : MATRIX_)
				tokmatrix ((Daata) theCurrentPraatObjects -> list [i]. object);
			}
		} else if (kar == U'(') {
			newchar;
			if (kar == U'=') {
				newtok (END_)   // "(=" = final comment
			} else {
				oldchar;
				newtok (OPENING_PARENTHESIS_)
			}
		} else if (kar == U')') {
			newtok (CLOSING_PARENTHESIS_)
		} else if (kar == U'+') {
			newtok (ADD_)
		} else if (kar == U'-') {
			if (itok == 0 || lexan [itok]. symbol <= MINUS_) {
				newtok (MINUS_)
			} else {
				newtok (SUB_)
			}
		} else if (kar == U'*') {
			newchar;
			if (kar == U'*') {
				newtok (POWER_)   // "**" = "^"
			} else {
				oldchar;
				newtok (MUL_)
			}
		} else if (kar == U'/') {
			newchar;
			if (kar == U'=') {
				newtok (NE_)   // "/=" = "<>"
			} else {
				oldchar;
				newtok (RDIV_)
			}
		} else if (kar == U'=') {
			newtok (EQ_)   // "="
			newchar;
			if (kar != U'=') {
				oldchar;   // "==" = "="
			}
		} else if (kar == U'>') {
			newchar;
			if (kar == U'=') {
				newtok (GE_)
			} else {
				oldchar;
				newtok (GT_)
			}
		} else if (kar == U'<') {
			newchar;
			if (kar == U'=') {
				newtok (LE_)
			} else if (kar == U'>') {
				newtok (NE_)
			} else {
				oldchar;
				newtok (LT_)
			}
		} else if (kar == U'!') {
			newchar;
			if (kar == U'=') {
				newtok (NE_)   // "!=" = "<>"
			} else {
				oldchar;
				newtok (NOT_)
			}
		} else if (kar == U',') {
			newtok (COMMA_)
		} else if (kar == U':') {
			newtok (COLON_)
		} else if (kar == U';') {
			newtok (END_)
		} else if (kar == U'^') {
			newtok (POWER_)
		} else if (kar == U'@') {
			do {
				newchar;
			} while (Melder_isHorizontalSpace (kar));
			stringtokon;
			do stringtokchar while (Melder_isWordCharacter (kar) || kar == U'.');
			stringtokoff;
			oldchar;
			newtok (CALL_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'"') {
			/*
				String constant, straight-quote version.
			*/
			newchar;
			stringtokon;
			for (;;) {
				if (kar == U'\0')
					formulaError (U"No closing quote in string constant", ikar);
				if (kar == U'"') {
					newchar;
					if (kar == U'"')
						stringtokchar
					else break;
				} else {
					stringtokchar
				}
			}
			stringtokoff;
			oldchar;
			newtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'“') {
			/*
				String constant, curly-quote version.
			*/
			newchar;
			stringtokon;
			for (;;) {
				if (kar == U'\0')
					formulaError (U"No closing quote in string constant", ikar);
				if (kar == U'”')
					break;
				stringtokchar
			}
			stringtokoff;
			newtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'~') {
			/*
				The content of the remainder of the line,
				including any leading and trailing space,
				will become a string constant (this is good for formulas).
			*/
			newchar;
			stringtokon;
			for (;;) {
				if (kar == U'\0')
					break;
				stringtokchar
			}
			stringtokoff;
			oldchar;
			newtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'|') {
			newtok (OR_)   /* "|" = "or" */
			newchar;
			if (kar != U'|') {
				oldchar;   /* "||" = "or" */
			}
		} else if (kar == U'&') {
			newtok (AND_)   /* "&" = "and" */
			newchar;
			if (kar != U'&') {
				oldchar;   /* "&&" = "and" */
			}
		} else if (kar == U'[') {
			newtok (OPENING_BRACKET_)
		} else if (kar == U']') {
			newtok (CLOSING_BRACKET_)
		} else if (kar == U'{') {
			newtok (OPENING_BRACE_)
		} else if (kar == U'}') {
			newtok (CLOSING_BRACE_)
		} else if (kar == U'.') {
			newtok (PERIOD_)
		} else {
			formulaError (U"Unknown symbol", ikar);
		}
	} while (lexan [itok]. symbol != END_);
}

static void fit (integer symbol) {
	if (symbol == newread) {
		return;   // success
	} else {
		const conststring32 symbolName1 = Formula_instructionNames [symbol];
		const conststring32 symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
		const bool needQuotes1 = ! str32chr (symbolName1, U' ');
		const bool needQuotes2 = ! str32chr (symbolName2, U' ');
		static MelderString message;
		MelderString_copy (& message,
			U"Expected ", ( needQuotes1 ? U"“" : nullptr ), symbolName1, ( needQuotes1 ? U"”" : nullptr ),
			U", but found ", ( needQuotes2 ? U"“" : nullptr ), symbolName2, ( needQuotes2 ? U"”" : nullptr ));
		formulaError (message.string, lexan [ilexan]. position);
	}
}

static bool fitArguments () {
    const integer symbol = newread;
    if (symbol == OPENING_PARENTHESIS_)
    	return true;   // success: a function call like: myFunction (...)
    if (symbol == COLON_)
    	return false;   // success: a function call like: myFunction: ...
    const conststring32 symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
    const bool needQuotes2 = ! str32chr (symbolName2, U' ');
    static MelderString message;
    MelderString_copy (& message,
		U"Expected “(” or “:”, but found ", ( needQuotes2 ? U"“" : nullptr ), symbolName2, ( needQuotes2 ? U"”" : nullptr ));
    formulaError (message.string, lexan [ilexan]. position);
    return false;   // will never occur
}

#define newparse(s)  parse [++ iparse]. symbol = (s)
#define parsenumber(g)  parse [iparse]. content.number = (g)
#define parselabel(l)  parse [iparse]. content.label = (l)

static void parseExpression ();

static void parsePowerFactor () {
	integer symbol = newread;

	if (symbol >= LOW_VALUE && symbol <= HIGH_VALUE) {
		newparse (symbol);
		if (symbol == NUMBER_) parsenumber (lexan [ilexan]. content.number);
		return;
	}

	if (symbol == STRING_) {
		newparse (symbol);
		parse [iparse]. content.string = lexan [ilexan]. content.string;   // reference copy!
		return;
	}

	if (symbol == NUMERIC_VARIABLE_ || symbol == STRING_VARIABLE_) {
		newparse (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return;
	}

	if (symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_) {
		char32 *var = lexan [ilexan]. content.string;   // Save before incrementing ilexan.
		if (newread == OPENING_BRACKET_) {
			int n = 0;
			if (newread != CLOSING_BRACKET_) {
				oldread;
				parseExpression ();
				n ++;
				while (newread == COMMA_) {
					parseExpression ();
					n ++;
				}
				oldread;
				fit (CLOSING_BRACKET_);
			}
			newparse (NUMBER_); parsenumber (n);
			newparse (symbol);
		} else {
			Melder_crash (U"Formula:parsePowerFactor (indexed variable): No '['; cannot happen.");
		}
		parse [iparse]. content.string = var;
		return;
	}

	if (symbol == NUMERIC_VECTOR_VARIABLE_) {
		const InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (VEC_CELL_);
		} else {
			oldread;
			newparse (NUMERIC_VECTOR_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == NUMERIC_MATRIX_VARIABLE_) {
		const InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (MAT_CELL_);
		} else {
			oldread;
			newparse (NUMERIC_MATRIX_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == STRING_ARRAY_VARIABLE_) {
		const InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (STRVEC_CELL_);
		} else {
			oldread;
			newparse (STRING_ARRAY_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == VARIABLE_NAME_) {
		const InterpreterVariable var = Interpreter_hasVariable (theInterpreter, lexan [ilexan]. content.string);
		if (! var)
			formulaError (U"Unknown variable", lexan [ilexan]. position);
		newparse (NUMERIC_VARIABLE_);
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == SELF_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFMATRIX2_);
				fit (CLOSING_BRACKET_);
				return;
			}
			oldread;
			newparse (SELFMATRIX1_);
			fit (CLOSING_BRACKET_);
			return;
		}
		if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFFUNCTION2_);
				fit (CLOSING_PARENTHESIS_);
				return;
			}
			oldread;
			newparse (SELFFUNCTION1_);
			fit (CLOSING_PARENTHESIS_);
			return;
		}
		oldread;
		newparse (SELF0_);
		return;
	}

	if (symbol == SELF_STR_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFMATRIX2_STR_);
				fit (CLOSING_BRACKET_);
				return;
			}
			oldread;
			newparse (SELFMATRIX1_STR_);
			fit (CLOSING_BRACKET_);
			return;
		}
		if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFFUNCTION2_STR_);
				fit (CLOSING_PARENTHESIS_);
				return;
			}
			oldread;
			newparse (SELFFUNCTION1_STR_);
			fit (CLOSING_PARENTHESIS_);
			return;
		}
		oldread;
		newparse (SELFSTR0_);
		return;
	}

	if (symbol == OBJECT_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_BRACKET_) {
				symbol = newread;
				if (symbol == PERIOD_) {
					switch (newread) {
						case XMIN_:
							newparse (OBJECT_XMIN_);
							return;
						case XMAX_:
							newparse (OBJECT_XMAX_);
							return;
						case YMIN_:
							newparse (OBJECT_YMIN_);
							return;
						case YMAX_:
							newparse (OBJECT_YMAX_);
							return;
						case NX_:
							newparse (OBJECT_NX_);
							return;
						case NY_:
							newparse (OBJECT_NY_);
							return;
						case DX_:
							newparse (OBJECT_DX_);
							return;
						case DY_:
							newparse (OBJECT_DY_);
							return;
						case NROW_:
							newparse (OBJECT_NROW_);
							return;
						case NCOL_:
							newparse (OBJECT_NCOL_);
							return;
						case ROW_STR_:
							fit (OPENING_BRACKET_);
							parseExpression ();
							newparse (OBJECT_ROW_STR_);
							fit (CLOSING_BRACKET_);
							return;
						case COL_STR_:
							fit (OPENING_BRACKET_);
							parseExpression ();
							newparse (OBJECT_COL_STR_);
							fit (CLOSING_BRACKET_);
							return;
						default:
							formulaError (U"After “object [number].” there should be “xmin”, “xmax”, “ymin”, "
								"“ymax”, “nx”, “ny”, “dx”, “dy”, “nrow” or “ncol”", lexan [ilexan]. position);
					}
				} else if (symbol == OPENING_BRACKET_) {
					parseExpression ();
					if (newread == COMMA_) {
						parseExpression ();
						newparse (OBJECTCELL2_);
						fit (CLOSING_BRACKET_);
					} else {
						oldread;
						newparse (OBJECTCELL1_);
						fit (CLOSING_BRACKET_);
					}
				} else {
					oldread;
					newparse (OBJECTCELL0_);
				}
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTCELL2_);
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (OBJECTCELL1_);
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (OBJECTLOCATION0_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTLOCATION2_);
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (OBJECTLOCATION1_);
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else {
			formulaError (U"After “object” there should be “(” or “[”", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == OBJECT_STR_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_BRACKET_) {
				newparse (OBJECTCELL0_STR_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTCELL2_STR_);
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (OBJECTCELL1_STR_);
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (OBJECTLOCATION0_STR_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTLOCATION2_STR_);
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (OBJECTLOCATION1_STR_);
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else {
			formulaError (U"After “object$” there should be “(” or “[”", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == OPENING_PARENTHESIS_) {
		parseExpression ();
		fit (CLOSING_PARENTHESIS_);
		return;
	}

	if (symbol == IF_) {
		const integer elseLabel = newlabel;   // has to be local,
		const integer endifLabel = newlabel;   // because of recursion
		parseExpression ();
		newparse (IFFALSE_);
		parselabel (elseLabel);
		fit (THEN_);
		parseExpression ();
		newparse (GOTO_);
		parselabel (endifLabel);
		fit (ELSE_);
		newparse (LABEL_);
		parselabel (elseLabel);
		parseExpression ();
		fit (ENDIF_);
		newparse (LABEL_);
		parselabel (endifLabel);
		return;
	}

	if (symbol == MATRIX_) {
		const Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			if (newread == CLOSING_BRACKET_) {
				newparse (MATRIX0_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (MATRIX2_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (MATRIX1_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (FUNCTION0_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (FUNCTION2_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (FUNCTION1_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else if (symbol == PERIOD_) {
			switch (newread) {
				case XMIN_:
					if (! thy v_hasGetXmin ()) {
						formulaError (U"Attribute “xmin” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getXmin ());
						return;
					}
				case XMAX_:
					if (! thy v_hasGetXmax ()) {
						formulaError (U"Attribute “xmax” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getXmax ());
						return;
					}
				case YMIN_:
					if (! thy v_hasGetYmin ()) {
						formulaError (U"Attribute “ymin” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getYmin ());
						return;
					}
				case YMAX_:
					if (! thy v_hasGetYmax ()) {
						formulaError (U"Attribute “ymax” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getYmax ());
						return;
					}
				case NX_:
					if (! thy v_hasGetNx ()) {
						formulaError (U"Attribute “nx” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNx ());
						return;
					}
				case NY_:
					if (! thy v_hasGetNy ()) {
						formulaError (U"Attribute “ny” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNy ());
						return;
					}
				case DX_:
					if (! thy v_hasGetDx ()) {
						formulaError (U"Attribute “dx” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getDx ());
						return;
					}
				case DY_:
					if (! thy v_hasGetDy ()) {
						formulaError (U"Attribute “dy” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getDy ());
						return;
					}
				case NCOL_:
					if (! thy v_hasGetNcol ()) {
						formulaError (U"Attribute “ncol” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNcol ());
						return;
					}
				case NROW_:
					if (! thy v_hasGetNrow ()) {
						formulaError (U"Attribute “nrow” not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNrow ());
						return;
					}
				case ROW_STR_:
					if (! thy v_hasGetRowStr ()) {
						formulaError (U"Attribute “row$” not defined for this object", lexan [ilexan]. position);
					} else {
						fit (OPENING_BRACKET_);
						parseExpression ();
						newparse (ROW_STR_);
						parse [iparse]. content.object = thee;
						fit (CLOSING_BRACKET_);
						return;
					}
				case COL_STR_:
					if (! thy v_hasGetColStr ()) {
						formulaError (U"Attribute “col$” not defined for this object", lexan [ilexan]. position);
					} else {
						fit (OPENING_BRACKET_);
						parseExpression ();
						newparse (COL_STR_);
						parse [iparse]. content.object = thee;
						fit (CLOSING_BRACKET_);
						return;
					}
				default: formulaError (U"Unknown attribute.", lexan [ilexan]. position);
			}
		} else {
			formulaError (U"After a name of a matrix there should be “(”, “[”, or “.”", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == MATRIX_STR_) {
		const Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			if (newread == CLOSING_BRACKET_) {
				newparse (MATRIX0_STR_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (MATRIX2_STR_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (MATRIX1_STR_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				}
			}
		} else {
			formulaError (U"After a name of a matrix$ there should be “[”", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol >= LOW_FUNCTION_1 && symbol <= HIGH_FUNCTION_1) {
		const bool isParenthesis = fitArguments ();
		parseExpression ();
		if (isParenthesis)
			fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_2 && symbol <= HIGH_FUNCTION_2) {
		const bool isParenthesis = fitArguments ();
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		if (isParenthesis)
			fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_3 && symbol <= HIGH_FUNCTION_3) {
		const bool isParenthesis = fitArguments ();
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		if (isParenthesis)
			fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_4 && symbol <= HIGH_FUNCTION_4) {
		const bool isParenthesis = fitArguments ();
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		if (isParenthesis)
			fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_N && symbol <= HIGH_FUNCTION_N) {
		int n = 0;
		const bool isParenthesis = fitArguments ();
		if (newread != CLOSING_PARENTHESIS_) {
			oldread;
			parseExpression ();
			n ++;
			while (newread == COMMA_) {
				parseExpression ();
				n ++;
			}
			oldread;
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		}
		newparse (NUMBER_); parsenumber (n);
		newparse (symbol);
		return;
	}

	if (symbol == OPENING_BRACE_) {
		if (newread == CLOSING_BRACE_) {
			newparse (NUMBER_);
			parsenumber (0);
			newparse (TENSOR_LITERAL_);
			return;
		}
		oldread;
		parseExpression ();
		int n = 1;
		while (newread == COMMA_) {
			parseExpression ();
			n ++;
		}
		oldread;
		fit (CLOSING_BRACE_);
		newparse (NUMBER_);
		parsenumber (n);
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			//fit (COMMA_);   // this would be for matrices
			//parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (TENSOR_LITERAL_CELL_);
		} else {
			oldread;
			newparse (TENSOR_LITERAL_);
		}
		return;
	}

	if (symbol == CALL_) {
		char32 *procedureName = lexan [ilexan]. content.string;   // reference copy!
		int n = 0;
		const bool isParenthesis = fitArguments ();
		if (newread != CLOSING_PARENTHESIS_) {
			oldread;
			parseExpression ();
			n ++;
			while (newread == COMMA_) {
				parseExpression ();
				n ++;
			}
			oldread;
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		}
		newparse (NUMBER_); parsenumber (n);
		newparse (CALL_);
		parse [iparse]. content.string = procedureName;
		return;
	}

	if (symbol >= LOW_STRING_FUNCTION && symbol <= HIGH_STRING_FUNCTION) {
		if (symbol >= LOW_FUNCTION_STR1 && symbol <= HIGH_FUNCTION_STR1) {
			const bool isParenthesis = fitArguments ();
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		} else if (symbol >= LOW_FUNCTION_STR2 && symbol <= HIGH_FUNCTION_STR2) {
			const bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		} else if (symbol >= LOW_FUNCTION_STR0 && symbol <= HIGH_FUNCTION_STR0) {
			fit (OPENING_PARENTHESIS_);
			fit (CLOSING_PARENTHESIS_);
		} else if (symbol == EXTRACT_WORD_STR_ || symbol == EXTRACT_LINE_STR_) {
			const bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		} else if (symbol == FIXED_STR_ || symbol == PERCENT_STR_ || symbol == HEXADECIMAL_STR_) {
			const bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		} else if (symbol == REPLACE_STR_ || symbol == REPLACE_REGEX_STR_) {
			const bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
		} else {
			oldread;   // needed for retry if we are going to be in a string comparison?
			formulaError (U"Function expected", lexan [ilexan + 1]. position);
		}
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_RANGE_FUNCTION && symbol <= HIGH_RANGE_FUNCTION) {
		if (symbol == SUM_OVER_) {
			//theOptimize = 1;
			newparse (NUMBER_);
			parsenumber (0.0);   // initialize the sum
			const bool isParenthesis = fitArguments ();
			const integer symbol2 = newread;
			if (symbol2 == NUMERIC_VARIABLE_) {   // an existing variable
				newparse (VARIABLE_REFERENCE_);
				InterpreterVariable loopVariable = lexan [ilexan]. content.variable;
				parse [iparse]. content.variable = loopVariable;
			} else if (symbol2 == VARIABLE_NAME_) {   // a new variable
				InterpreterVariable loopVariable = Interpreter_lookUpVariable (theInterpreter, lexan [ilexan]. content.string);
				newparse (VARIABLE_REFERENCE_);
				parse [iparse]. content.variable = loopVariable;
			} else {
				formulaError (U"Numeric variable expected", lexan [ilexan]. position);
			}
			// now on stack: sum, loop variable
			if (newread == FROM_) {
				parseExpression ();
			} else {
				oldread;
				newparse (NUMBER_);
				parsenumber (1.0);
			}
			newparse (DECREMENT_AND_ASSIGN_);   // this pushes the variable back on the stack
			// now on stack: sum, loop variable
			fit (TO_);
			parseExpression ();
			// now on stack: sum, loop variable, end value
			const integer startLabel = newlabel;
			const integer endLabel = newlabel;
			newparse (LABEL_);
			parselabel (startLabel);
			newparse (INCREMENT_GREATER_GOTO_);
			parselabel (endLabel);
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis)
				fit (CLOSING_PARENTHESIS_);
			// now on stack: sum, loop variable, end value, value to add
			newparse (ADD_3DOWN_);
			// now on stack: sum, loop variable, end value
			newparse (GOTO_);
			parselabel (startLabel);
			newparse (LABEL_);
			parselabel (endLabel);
			newparse (POP_2_);
			// now on stack: sum
			return;
		}
	}

	if (symbol == STOPWATCH_) {
		newparse (symbol);
		return;
	}

	oldread;   // needed for retry if we are going to be in a string comparison
	formulaError (U"Symbol misplaced", lexan [ilexan + 1]. position);
}

static void parseFactor ();

static void parsePowerFactors () {
	if (newread == POWER_) {
		if (ilexan > 2 && lexan [ilexan - 2]. symbol == MINUS_ && lexan [ilexan - 1]. symbol == NUMBER_) {
			oldread;
			formulaError (U"Expressions like -3^4 are ambiguous; use (-3)^4 or -(3^4) or -(3)^4", lexan [ilexan + 1]. position);
		}
		parseFactor ();   // like a^-b
		newparse (POWER_);
	}
	else
		oldread;
}

static void parseMinus () {
	parsePowerFactor ();
	parsePowerFactors ();
}

static void parseFactor () {
	if (newread == MINUS_) {
		parseFactor ();   // there can be multiple consecutive minuses
		newparse (MINUS_);
		return;
	}
	oldread;
	parseMinus ();   // like -a^b
}

static void parseFactors () {
	const integer symbol = newread;   // has to be local, because of recursion
	if (symbol == MUL_ || symbol == RDIV_ || symbol == IDIV_ || symbol == MOD_) {
		parseFactor ();
		newparse (symbol);
		parseFactors ();
	}
	else oldread;
}

static void parseTerm () {
	parseFactor ();
	parseFactors ();
}

static void parseTerms () {
	const integer symbol = newread;
	if (symbol == ADD_ || symbol == SUB_) {
		parseTerm ();
		newparse (symbol);
		parseTerms ();
	}
	else oldread;
}

static void parseNot () {
	parseTerm ();
	parseTerms ();
	integer symbol = newread;
	if (symbol >= EQ_ && symbol <= GT_) {
		parseTerm ();
		parseTerms ();
		newparse (symbol);
	}
	else oldread;
}

static void parseAnd () {
	if (newread == NOT_) {
		parseAnd ();   // like not not not
		newparse (NOT_);
		return;
	}
	oldread;
	parseNot ();
}

static void parseOr () {
	parseAnd ();
	if (newread == AND_) {
		integer falseLabel = newlabel;
		integer andLabel = newlabel;
		do {
			newparse (IFFALSE_); parselabel (falseLabel);
			parseAnd ();
		} while (newread == AND_);
		newparse (IFFALSE_); parselabel (falseLabel);
		newparse (TRUE_);
		newparse (GOTO_); parselabel (andLabel);
		newparse (LABEL_); parselabel (falseLabel);
		newparse (FALSE_);
		newparse (LABEL_); parselabel (andLabel);
	}
	oldread;
}

static void parseExpression () {
	parseOr ();
	if (newread == OR_) {
		integer trueLabel = newlabel;
		integer orLabel = newlabel;
		do {
			newparse (IFTRUE_); parselabel (trueLabel);
			parseOr ();
		} while (newread == OR_);
		newparse (IFTRUE_); parselabel (trueLabel);
		newparse (FALSE_);
		newparse (GOTO_); parselabel (orLabel);
		newparse (LABEL_); parselabel (trueLabel);
		newparse (TRUE_);
		newparse (LABEL_); parselabel (orLabel);
	}
	oldread;
}

/*
	Translate the infix expression "my lexan" into the postfix expression "my parse":
	remove parentheses and brackets, commas, colons, FROM_, TO_,
	IF_ THEN_ ELSE_ ENDIF_ OR_ AND_;
	introduce LABEL_ GOTO_ IFTRUE_ IFFALSE_ TRUE_ FALSE_
	SELF0_ SELF1_ SELF2_ MATRIX0_ MATRIX1_ MATRIX2_
	Return:
		0 if error, otherwise 1.
	Precondition:
		"my lexan" contains an END_ symbol.
	Postconditions:
		*my lexan not changed.
		result == 0 || my parse [my numberOfInstructions]. symbol == END_
*/

static void Formula_parseExpression () {
	ilabel = ilexan = iparse = 0;
	if (lexan [1]. symbol == END_)
		Melder_throw (U"Empty formula.");
	parseExpression ();
	fit (END_);
	newparse (END_);
	numberOfInstructions = iparse;
}

static void shift (integer begin, integer distance) {
	numberOfInstructions -= distance;
	for (integer j = begin; j <= numberOfInstructions; j ++)
		parse [j] = parse [j + distance];
}

static integer findLabel (integer label) {
	integer result = numberOfInstructions;
	while (parse [result]. symbol != LABEL_ ||
			 parse [result]. content.label != label)
		result --;
	return result;
}

static void Formula_optimizeFlow ()
/* Vereenvoudig boolse uitdrukkingen.					*/
/* Nadien:								*/
/*    de stroom volgt het kortste pad;					*/
/*    als de rekenkundige waarden van boolse uitdrukkingen		*/
/*    in de formule niet voorkomen, zijn alle TRUE_s en FALSE_s weg;	*/
/*    als in de formule geen NOT_s voorkwamen op rekenkundige		*/
/*    uitdrukkingen, zijn alle NOT_s weg;				*/
/*    onbereikbare kode is weg;						*/
{
	integer i, j, volg;
	for (;;) {
		bool improved = false;
		for (i = 1; i <= numberOfInstructions; i ++)
		{

/* Optimalisatie 1: */
/*    true   goto x  ->  goto y  /  __  ...  label x  iftrue y    */
/*    false  goto x  ->  goto y  /  __  ...  label x  iffalse y   */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_))
			{
				 improved = true;
				 parse [i]. symbol = GOTO_;
				 parse [i]. content.label = parse [volg]. content.label;
				 shift (i + 1, 1);
			}

/* Optimalisatie 2: */
/*    true  goto x  ...  label x  iffalse y  ->        */
/*          goto z  ...  label x  iffalse y  label z   */
/*    en analoog met false en iftrue. */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_))
			{
				improved = true;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = newlabel;
				for (j = i + 1; j < volg; j ++)
					parse [j] = parse [j + 1];
				parse [volg]. symbol = LABEL_;
				parse [volg]. content.label = ilabel;
			}

/* Optimalisatie 3a: */
/*    iftrue x  goto y  label x  ->  iffalse y  label x   */

			if (parse [i]. symbol == IFTRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [i + 2]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 2]. content.label)
			{
				improved = true;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 3b: */
/*    iffalse x  goto y  label x  ->  iftrue y  label x   */

			if (parse [i]. symbol == IFFALSE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [i + 2]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 2]. content.label)
			{
				improved = true;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 4: */
/*    verwijder onbereikbare kode: na een GOTO_ hoort een LABEL_. */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i + 1]. symbol != LABEL_)
			{
				improved = true;
				j = i + 2;
				while (parse [j]. symbol != LABEL_) j ++;
				shift (i + 1, j - i - 1);
			}

/* Optimalisatie 5: */
/*    goto x  ->  0  /  __  label x   */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 1]. content.label)
			{
				improved = true;
				shift (i, 1);
			}

/* Optimalisatie 6: */
/*    true   iffalse x  ->  0  */
/*    false  iftrue x   ->  0  */

			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFFALSE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFTRUE_))
			{
				improved = true;
				shift (i, 2);
			}

/* Optimalisatie 7: */
/*    true   iftrue x   ->  goto x    */
/*    false  iffalse x  ->  goto x    */

			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFTRUE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFFALSE_))
			{
				improved = true;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 8: */
/*    iftrue x   ->  iftrue y   /  __  ...  label x  goto y   */
/*    iffalse x  ->  iffalse y  /  __  ...  label x  goto y   */

			if ((parse [i]. symbol == IFTRUE_ || parse [i]. symbol == IFFALSE_)
				&& parse [volg = findLabel (parse [i]. content.label) + 1]. symbol == GOTO_)
			{
				improved = true;
				parse [i]. content.label = parse [volg]. content.label;
			}

/* Optimalisatie 9a: */
/*    not  iftrue x  ->  iffalse x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFTRUE_)
			{
				improved = true;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 9b: */
/*    not  iffalse x  ->  iftrue x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFFALSE_)
			{
				improved = true;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* De volgende optimalisaties ontbreken want zijn hier overbodig: */
/*    goto x  ->  goto y  /  __  ... label x  goto y   */
/*    trek twee opeenvolgende labels samen             */

		} /* for i */

		/* Verwijder labels waar niet naar verwezen wordt. */

		for (i = 1; i <= numberOfInstructions; i ++)
			if (parse [i]. symbol == LABEL_)
			{
				bool found = false;
				for (j = 1; j <= numberOfInstructions; j ++)
					if ((parse [j]. symbol == GOTO_ || parse [j]. symbol == IFFALSE_ || parse [j]. symbol == IFTRUE_
						|| parse [j]. symbol == INCREMENT_GREATER_GOTO_)
						&& parse [i]. content.label == parse [j]. content.label)
						found = true;
				if (! found)
				{
					improved = true;
					shift (i, 1);
				}
			}
		if (! improved)
			break;
	}
}

static integer praat_findObjectById (integer id) {
	integer IOBJECT;
	WHERE_DOWN (ID == id)
		return IOBJECT;
	Melder_throw (U"No object with number ", id, U".");
}

static integer praat_findObjectByName (conststring32 name) {
	integer IOBJECT;
	if (*name >= U'A' && *name <= U'Z') {
		static MelderString buffer;
		MelderString_copy (& buffer, name);
		char32 *spaceLocation = str32chr (buffer.string, U' ');
		if (! spaceLocation)
			Melder_throw (U"Missing space in object name “", name, U"”.");
		*spaceLocation = U'\0';
		conststring32 className = & buffer.string [0], givenName = spaceLocation + 1;
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
				return IOBJECT;
		}
		ClassInfo klas = Thing_classFromClassName (className, nullptr);
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (klas -> className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
				return IOBJECT;
		}
	}
	Melder_throw (U"No object with name “", name, U"”.");
}

static void Formula_evaluateConstants () {
	for (;;) {
		bool improved = false;
		for (integer i = 1; i <= numberOfInstructions; i ++) {
			integer gain = 0;
			if (parse [i]. symbol == NUMBER_) {
				if (parse [i]. content.number == 2.0 && parse [i + 1]. symbol == POWER_)
					{ gain = 1; parse [i]. symbol = SQR_; }
				else if (parse [i + 1]. symbol == MINUS_)
					{ gain = 1; parse [i]. content.number = - parse [i]. content.number; }
				else if (parse [i + 1]. symbol == SQR_)
					{ gain = 1; parse [i]. content.number *= parse [i]. content.number; }
				else if (parse [i + 1]. symbol == NUMBER_) {
					if (parse [i + 2]. symbol == ADD_)
						{ gain = 2; parse [i]. content.number += parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == SUB_)
						{ gain = 2; parse [i]. content.number -= parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == MUL_)
						{ gain = 2; parse [i]. content.number *= parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == RDIV_)
						{ gain = 2; parse [i]. content.number /= parse [i + 1]. content.number; }
				} else if (parse [i + 1]. symbol == TO_OBJECT_) {
					parse [i]. symbol = OBJECT_;
					integer IOBJECT = praat_findObjectById (Melder_iround (parse [i]. content.number));
					parse [i]. content.object = OBJECT;
					gain = 1;
				}
			} else if (parse [i]. symbol == STRING_) {
				if (parse [i + 1]. symbol == TO_OBJECT_) {
					parse [i]. symbol = OBJECT_;
					const integer IOBJECT = praat_findObjectByName (parse [i]. content.string);
					parse [i]. content.object = OBJECT;
					gain = 1;
				}
			} else if (parse [i]. symbol == NUMERIC_VARIABLE_) {
				parse [i]. symbol = NUMBER_;
				parse [i]. content.number = parse [i]. content.variable -> numericValue;
				gain = 0;
				improved = true;
			} else if (parse [i]. symbol == STRING_VARIABLE_) {
				parse [i]. symbol = STRING_;
				parse [i]. content.string = parse [i]. content.variable -> stringValue.get();   // again a reference copy (lexan is still the owner)
				gain = 0;
				improved = true;
			#if 0
			} else if (parse [i]. symbol == ROW_) {
				if (parse [i + 1]. symbol == COL_ && parse [i + 2]. symbol == SELFMATRIX2_)
					{ gain = 2; parse [i]. symbol = SELF0_; }   // TODO: SELF0_ may not have the same restrictions as SELFMATRIX2_
			} else if (parse [i]. symbol == COL_) {
				if (parse [i + 1]. symbol == SELFMATRIX1_)
					{ gain = 1; parse [i]. symbol = SELF0_; }
			#endif
			}
			if (gain > 0) {
				improved = true;
				shift (i + 1, gain);
			}
		}
		if (! improved)
			break;
	}
}

static void Formula_removeLabels () {
	/*
		First translate symbolic labels (< 0) into instructions locations (> 0).
	*/
	for (integer i = 1; i <= numberOfInstructions; i ++) {
		const integer symboli = parse [i]. symbol;
		if (symboli == GOTO_ || symboli == IFTRUE_ || symboli == IFFALSE_ || symboli == INCREMENT_GREATER_GOTO_) {
			integer label = parse [i]. content.label;
			for (int j = 1; j <= numberOfInstructions; j ++) {
				if (parse [j]. symbol == LABEL_ && parse [j]. content.label == label) {
					parse [i]. content.label = j;
				}
			}
		}
	}
	/*
		Then remove the labels,
		which have become superfluous.
	*/
	if (theOptimize) {
		integer i = 1;
		while (i <= numberOfInstructions) {
			const integer symboli = parse [i]. symbol;
			if (symboli == LABEL_) {
				shift (i, 1);   // remove one label
				for (int j = 1; j <= numberOfInstructions; j ++) {
					const integer symbolj = parse [j]. symbol;
					if ((symbolj == GOTO_ || symbolj == IFTRUE_ || symbolj == IFFALSE_ || symbolj == INCREMENT_GREATER_GOTO_) && parse [j]. content.label > i)
						parse [j]. content.label --;  /* Pas een label aan. */
				}
				i --;   // voorkom ophogen i (overbodig?)
			}
			i ++;
		}
	}
	numberOfInstructions --;   /* Het END_-symbol hoeft niet geinterpreteerd. */
}

#include <inttypes.h>

/*
	For debugging.
*/
static void Formula_print (FormulaInstruction f) {
	integer i = 0, symbol;
	do {
		conststring32 instructionName;
		symbol = f [++ i]. symbol;
		instructionName = Formula_instructionNames [symbol];
		if (symbol == NUMBER_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_ || symbol == INCREMENT_GREATER_GOTO_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string.get(), U" ", f [i]. content.variable -> numericValue);
		else if (symbol == STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string.get(), U" ", f [i]. content.variable -> stringValue.get());
		else if (symbol == STRING_ || symbol == VARIABLE_NAME_ || symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.string);
		else if (symbol == MATRIX_ || symbol == MATRIX_STR_ || symbol == MATRIX1_ || symbol == MATRIX1_STR_ ||
		         symbol == MATRIX2_ || symbol == MATRIX2_STR_ || symbol == ROW_STR_ || symbol == COL_STR_)
		{
			Thing object = f [i]. content.object;
			if (object) {
				Melder_casual (i, U" ", instructionName, U" ", Thing_className (object), U" ", object -> name.get());
			} else {
				Melder_casual (i, U" ", instructionName);
			}
		}
		else if (symbol == CALL_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.string);
		else
			Melder_casual (i, U" ", instructionName);
	} while (symbol != END_);
}

void Formula_compile (Interpreter interpreter, Daata data, conststring32 expression, int expressionType, bool optimize) {
	if (interpreter)
		Melder_assert (interpreter -> owningInterpreterStack);
	theInterpreter = interpreter;
	if (! theInterpreter) {
		if (! theLocalInterpreterStack) {
			theLocalInterpreterStack = InterpreterStack_create (nullptr);
			theLocalInterpreterStack -> interpreters [1] = Interpreter_createFromEnvironment (
				theLocalInterpreterStack.get(),
				Editor (nullptr),
				MelderFile (nullptr)
			);
		}
		theInterpreter = theLocalInterpreterStack -> interpreters [1].get();
		theInterpreter -> owningInterpreterStack = theLocalInterpreterStack.get();
		theInterpreter -> variablesMap. clear ();
	}
	theSource = data;
	theExpression = expression;
	theExpressionType [theLevel] = expressionType;
	theOptimize = optimize;
	if (! lexan) {
		lexan = Melder_calloc_f (structFormulaInstruction, Formula_MAXIMUM_STACK_SIZE);
		lexan [Formula_MAXIMUM_STACK_SIZE - 1]. symbol = END_;   // make sure that cleaning up always terminates
	}
	if (! parse)
		parse = Melder_calloc_f (structFormulaInstruction, Formula_MAXIMUM_STACK_SIZE);

	/*
		Clean up strings from the previous call.
		These strings are in a union, that's why this cannot be done later, when a new string is created.
	*/
	if (numberOfStringConstants) {
		ilexan = 1;
		for (;;) {
			integer symbol = lexan [ilexan]. symbol;
			if (symbol == STRING_ ||
				symbol == VARIABLE_NAME_ ||
				symbol == INDEXED_NUMERIC_VARIABLE_ ||
				symbol == INDEXED_STRING_VARIABLE_ ||
				symbol == CALL_
			) {
				Melder_free (lexan [ilexan]. content.string);
			}
			else if (symbol == END_) break;   // either the end of a formula, or the end of lexan
			ilexan ++;
		}
		numberOfStringConstants = 0;
	}

	Formula_lexan ();
	if (Melder_debug == 17) Formula_print (lexan);
	Formula_parseExpression ();
	if (Melder_debug == 17) Formula_print (parse);
	if (theOptimize) {
		Formula_optimizeFlow ();
		if (Melder_debug == 17) Formula_print (parse);
		Formula_evaluateConstants ();
		if (Melder_debug == 17) Formula_print (parse);
	}
	Formula_removeLabels ();
	if (Melder_debug == 17) Formula_print (parse);
}

/*
	Running.
*/

conststring32 structStackel :: whichText () {
	return
		our which == Stackel_NUMBER ? U"a number" :
		our which == Stackel_NUMERIC_VECTOR ? U"a numeric vector" :
		our which == Stackel_NUMERIC_MATRIX ? U"a numeric matrix" :
		our which == Stackel_STRING ? U"a string" :
		our which == Stackel_STRING_ARRAY ? U"a string vector" :
		our which == Stackel_OBJECT ? U"an object" :
		U"???";
}

static integer programPointer;

static Stackel theStack;
static integer stackPointer, stackPointerMax;
#define pop  & theStack [stackPointer --]
#define topOfStack  & theStack [stackPointer]
inline static void pushNumber (const double x) {
	/* inline runs 10 to 20 percent faster; here's the test script:
		stopwatch
		Create Sound from formula: "test", 1, 0.0, 1000.0, 44100, ~ x + 1 + 2 + 3 + 4 + 5 + 6
		writeInfoLine: stopwatch
		Remove
	 * Mac: 3.76 -> 3.20 seconds
	 */
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_NUMBER;
	stackel -> number = ( isdefined (x) ? x : undefined );
	//stackel -> number = x;   // this one would be 2 percent faster
	//stackel -> owned = true;   // superfluous, because never checked (2020-12-20)
}
static void pushNumericVector (autoVEC x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_NUMERIC_VECTOR;
	stackel -> numericVector = x.releaseToAmbiguousOwner();
	stackel -> owned = true;
}
static void pushNumericVectorReference (VEC x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_NUMERIC_VECTOR;
	stackel -> numericVector = x;
	stackel -> owned = false;
}
static void pushNumericMatrix (autoMAT x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_NUMERIC_MATRIX;
	stackel -> numericMatrix = x.releaseToAmbiguousOwner();
	stackel -> owned = true;
}
static void pushNumericMatrixReference (MAT x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_NUMERIC_MATRIX;
	stackel -> numericMatrix = x;
	stackel -> owned = false;
}
static void pushString (autostring32 x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	//stackel -> reset();   // incorporated in next statement
	stackel -> setString (x.move());
	//stackel -> owned = true;   // superfluous, because never checked (2020-12-20)
}
static void pushStringVector (autoSTRVEC x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_STRING_ARRAY;
	stackel -> stringArray = x.releaseToAmbiguousOwner();
	stackel -> owned = true;
}
static void pushStringVectorReference (STRVEC x) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_STRING_ARRAY;
	stackel -> stringArray = x;
	stackel -> owned = false;
}
static void pushObject (Daata object) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_OBJECT;
	stackel -> object = object;
	//stackel -> owned = false;   // superfluous, because never checked (2020-12-20)
}
static void pushVariable (InterpreterVariable var) {
	if (++ stackPointer > stackPointerMax)
		if (++ stackPointerMax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	const Stackel stackel = & theStack [stackPointer];
	stackel -> reset();
	stackel -> which = Stackel_VARIABLE;
	stackel -> variable = var;
	//stackel -> owned = false;   // superfluous, because never checked (2020-12-20)
}

static void do_not () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number == 0.0 ? 1.0 : 0.0);
	} else {
		Melder_throw (U"Cannot negate (“not”) ", x->whichText(), U".");
	}
}
static void do_eq () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		/*
			It is possible that we are comparing a value against --undefined--.
			Any defined value is unequal to --undefined--,
			but any undefined value (inf or NaN) *is* equal to --undefined--.
			Note that this is different from how "==" works in C.
		*/
		pushNumber (NUMequal (x->number, y->number) ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		pushNumber (str32equ (x->getString(), y->getString()) ? 1.0 : 0.0);
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMequal (x->numericVector, y->numericVector) ? 1.0 : 0.0);
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMequal (x->numericMatrix, y->numericMatrix) ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING_ARRAY && y->which == Stackel_STRING_ARRAY) {
		pushNumber (NUMequal (x->stringArray, y->stringArray) ? 1.0 : 0.0);
	} else {
		Melder_throw (U"Cannot compare (=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_ne () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		/*
			Unequal is defined as the opposite of equal.
		*/
		pushNumber (NUMequal (x->number, y->number) ? 0.0 : 1.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		pushNumber (str32equ (x->getString(), y->getString()) ? 0.0 : 1.0);
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMequal (x->numericVector, y->numericVector) ? 0.0 : 1.0);
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMequal (x->numericMatrix, y->numericMatrix) ? 0.0 : 1.0);
	} else if (x->which == Stackel_STRING_ARRAY && y->which == Stackel_STRING_ARRAY) {
		pushNumber (NUMequal (x->stringArray, y->stringArray) ? 0.0 : 1.0);
	} else {
		Melder_throw (U"Cannot compare (<>) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_le () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number <= y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not equal to, nor less than, undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not equal to, nor less than, defined
			} else {
				pushNumber (1.0);   // undefined is equal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		const double result = str32cmp (x->getString(), y->getString()) <= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_lt () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number < y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not less than undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not less than defined
			} else {
				pushNumber (0.0);   // undefined is not less than undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		const double result = str32cmp (x->getString(), y->getString()) < 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_ge () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		const double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number >= y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not equal to, nor greater than, undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not equal to, nor greater than, defined
			} else {
				pushNumber (1.0);   // undefined is equal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		const double result = str32cmp (x->getString(), y->getString()) >= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_gt () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		const double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number > y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not greater than undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not greater than defined
			} else {
				pushNumber (0.0);   // undefined is not greater than undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		const double result = str32cmp (x->getString(), y->getString()) > 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
inline static void moveNumericVector (Stackel from, Stackel to) {
	//Melder_assert (from -> owned);
	//Melder_assert (to -> which == Stackel_NUMERIC_VECTOR);
	from -> owned = false;
	to -> numericVector = from -> numericVector;
	to -> owned = true;
}
inline static void moveNumericMatrix (Stackel from, Stackel to) {
	//Melder_assert (from -> owned);
	//Melder_assert (to -> which == Stackel_NUMERIC_MATRIX);
	from -> owned = false;
	to -> numericMatrix = from -> numericMatrix;
	to -> owned = true;
}

static void do_add () {
	/*
		result.. = x.. + y..
	*/
	const Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*@praat
				#
				# result = x + y
				#
				x = 5
				y = 6
				result = x + y
				assert result = 11
			@*/
			x->number += y->number;
			//x->which = Stackel_NUMBER;   // superfluous, as is cleaning up
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x + y#
			*/
			if (y->owned) {
				/*@praat
					#
					# result# = x + owned y#
					#
					result# = 5 + { 11, 13, 31 }   ; numeric vector literals are owned
					assert result# = { 16, 18, 36 }
				@*/
				y->numericVector  +=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = x + unowned y#
					#
					y# = { 17, -11, 29 }
					result# = 30 + y#   ; numeric vector variables are not owned
					assert result# = { 47, 19, 59 }
				@*/
				// x does not have to be cleaned up, because it was a number
				x->numericVector = add_VEC (y->numericVector, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x + y##
			*/
			if (y->owned) {
				y->numericMatrix  +=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was a number
				x->numericMatrix = add_MAT (y->numericMatrix, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# + y#
				i.e.
				result# [i] = x# [i] + y# [i]
			*/
			const integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny) {
				/*@praat
					#
					# Error: unequal sizes.
					#
					x# = { 11, 13, 17 }
					y# = { 8, 90 }
					asserterror When adding vectors, their numbers of elements should be equal, instead of 3 and 2.
					result# = x# + y#
				@*/
				Melder_throw (U"When adding vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			}
			if (x -> owned) {
				/*@praat
					#
					# result# = owned x# + y#
					#
					result# = { 11, 13, 17 } + { 44, 56, 67 }   ; owned + owned
					assert result# = { 55, 69, 84 }
					y# = { 3, 2, 89.5 }
					result# = { 11, 13, 17 } + y#   ; owned + unowned
					assert result# = { 14, 15, 106.5 }
				@*/
				x->numericVector  +=  y->numericVector;
			} else if (y -> owned) {
				/*@praat
					#
					# result# = unowned x# + owned y#
					#
					x# = { 14, -3, 6.25 }
					result# = x# + { 55, 1, -89 }
					assert result# = { 69, -2, -82.75 }
				@*/
				y->numericVector  +=  x->numericVector;
				// x does not have to be cleaned up, because it was not owned
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = unowned x# + unowned y#
					#
					x# = { 14, -33, 6.25 }
					y# = { -33, 17, 9 }
					result# = x# + y#
					assert result# = { -19, -16, 15.25 }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = add_VEC (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x# + y##
				i.e.
				result## [i, j] = x# [i] + y## [i, j]
			*/
			const integer xsize = x->numericVector.size;
			const integer ynrow = y->numericMatrix.nrow;
			Melder_require (ynrow == xsize,
				U"When adding a matrix to a vector, the matrix’s number of rows should be equal to the vector’s size, "
				"instead of ", ynrow, U" and ", xsize, U"."
			);
			if (x->owned) {
				/*@praat
					assert { 1, 2, 3 } + { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 2, 3 }, { 5, 6 }, { 8, 9 } }
				@*/
				autoMAT newMatrix = add_MAT (x->numericVector, y->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix. releaseToAmbiguousOwner();
			} else {
				/*@praat
					a# = { 1, 2, 3 }
					assert a# + { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 2, 3 }, { 5, 6 }, { 8, 9 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = add_MAT (x->numericVector, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# + y
				i.e.
				result# [i] = x# [i] + y
			*/
			if (x->owned) {
				x->numericVector  +=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = add_VEC (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x## + y##
				i.e.
				result## [i, j] = x## [i, j] + y## [i, j]
			*/
			const integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			const integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When adding matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When adding matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  +=  y->numericMatrix;
			} else if (y->owned) {
				y->numericMatrix  +=  x->numericMatrix;
				// x does not have to be cleaned up, because it was not owned
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was not owned and has the right shape
				x->numericMatrix = add_MAT (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result## = x## + y#
				i.e.
				result## [i, j] = x## [i, j] + y# [j]
			*/
			Melder_require (y->numericVector.size == x->numericMatrix.ncol,
				U"Cannot add a vector with ", y->numericVector.size, U" elements "
				"to a matrix with ", x->numericMatrix.ncol, U" columns. "
				"These numbers should be equal."
			);
			if (x->owned) {
				/*@praat
					#
					# result## = owned x## + y#
					#
					y# = { -5, 6, -19 }
					result## = { { 14, -33, 6.25 }, { -33, 17, 9 } } + y#
					assert result## = { { 9, -27, -12.75 }, { -38, 23, -10 } }
				@*/
				x->numericMatrix  +=  y->numericVector;
			} else {
				/*@praat
					#
					# result## = unowned x## + y#
					#
					x## = { { 14, -33, 6.25 }, { -33, 17, 9 } }
					y# = { -5, 6, -19 }
					result## = x## + y#
					assert result## = { { 9, -27, -12.75 }, { -38, 23, -10 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = add_MAT (x->numericMatrix, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result## = x## + y
				i.e.
				result## [i, j] = x## [i, j] + y
			*/
			if (x->owned) {
				x->numericMatrix  +=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = add_MAT (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		/*
			result$ = x$ + y$
		*/
		const integer length1 = Melder_length (x->getString()), length2 = Melder_length (y->getString());
		autostring32 result (length1 + length2);
		str32cpy (result.get(), x->getString());
		str32cpy (result.get() + length1, y->getString());
		x->setString (result.move());
		return;
	}
	Melder_throw (U"Cannot add ", y->whichText(), U" to ", x->whichText(), U".");
}
static void do_sub () {
	/*
		result.. = x.. - y..
	*/
	const Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*@praat
				#
				# result = x - y
				#
				x = 5
				y = 6
				result = x - y
				assert result = -1
			@*/
			x->number -= y->number;
			//x->which = Stackel_NUMBER;   // superfluous, as is cleaning up
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x - y#
			*/
			if (y->owned) {
				/*@praat
					#
					# result# = x - owned y#
					#
					result# = 5 - { 11, 13, 31 }   ; numeric vector literals are owned
					assert result# = { -6, -8, -26 }
				@*/
				y->numericVector  <<=  x->number  -  y->numericVector;
				// x does not have to be cleaned up, because it was a number
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = x - unowned y#
					#
					y# = { 17, -11, 29 }
					result# = 30 - y#   ; numeric vector variables are not owned
					assert result# = { 13, 41, 1 }
				@*/
				// x does not have to be cleaned up, because it was a number
				x->numericVector = subtract_VEC (x->number, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x - y##
			*/
			if (y->owned) {
				subtractReversed_MAT_inout (y->numericMatrix, x->number);
				// x does not have to be cleaned up, because it was a number
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was a number
				x->numericMatrix = subtract_MAT (x->number, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# - y#
				i.e.
				result# [i] = x# [i] - y# [i]
			*/
			const integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny) {
				/*@praat
					#
					# Error: unequal sizes.
					#
					x# = { 11, 13, 17 }
					y# = { 8, 90 }
					asserterror When subtracting vectors, their numbers of elements should be equal, instead of 3 and 2.
					result# = x# - y#
				@*/
				Melder_throw (U"When subtracting vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			}
			if (x -> owned) {
				/*@praat
					#
					# result# = owned x# - y#
					#
					result# = { 11, 13, 17 } - { 44, 56, 67 }   ; owned + owned
					assert result# = { -33, -43, -50 }
					y# = { 3, 2, 89.5 }
					result# = { 11, 13, 17 } - y#   ; owned + unowned
					assert result# = { 8, 11, -72.5 }
				@*/
				x->numericVector  -=  y->numericVector;
			} else if (y -> owned) {
				/*@praat
					#
					# result# = unowned x# + owned y#
					#
					x# = { 14, -3, 6.25 }
					result# = x# + { 55, 1, -89 }
					assert result# = { 69, -2, -82.75 }
				@*/
				y->numericVector  <<=  x->numericVector  -  y->numericVector;
				// x does not have to be cleaned up, because it was not owned
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = unowned x# + unowned y#
					#
					x# = { 14, -33, 6.25 }
					y# = { -33, 17, 9 }
					result# = x# - y#
					assert result# = { 47, -50, -2.75 }
				@*/
				// no clean-up of x required, because x is not owned and has the right type
				x->numericVector = subtract_VEC (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x# - y##
				i.e.
				result## [i, j] = x# [i] - y## [i, j]
			*/
			const integer xsize = x->numericVector.size;
			const integer ynrow = y->numericMatrix.nrow;
			Melder_require (ynrow == xsize,
				U"When subtracting a matrix from a vector, the matrix’s number of rows should be equal to the vector’s size, "
				"instead of ", ynrow, U" and ", xsize, U"."
			);
			if (x->owned) {
				/*@praat
					assert { 1, 2, 3 } - { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 0, -1 }, { -1, -2 }, { -2, -3 } }
				@*/
				autoMAT newMatrix = subtract_MAT (x->numericVector, y->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix. releaseToAmbiguousOwner();
			} else {
				/*@praat
					a# = { 1, 2, 3 }
					assert a# - { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 0, -1 }, { -1, -2 }, { -2, -3 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = subtract_MAT (x->numericVector, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# - y
				i.e.
				result# [i] = x# [i] - y
			*/
			if (x->owned) {
				x->numericVector  -=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = subtract_VEC (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			const integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			const integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When subtracting matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When subtracting matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  -=  y->numericMatrix;
			} else if (y->owned) {
				subtractReversed_MAT_inout (y->numericMatrix, x->numericMatrix);
				// x does not have to be cleaned up, because it was not owned
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was not owned and has the right shape
				x->numericMatrix = subtract_MAT (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result## = x## - y#
				i.e.
				result## [i, j] = x## [i, j] - y# [j]
			*/
			Melder_require (y->numericVector.size == x->numericMatrix.ncol,
				U"Cannot subtract a vector with ", y->numericVector.size, U" elements "
				"from a matrix with ", x->numericMatrix.ncol, U" columns. "
				"These numbers should be equal."
			);
			if (x->owned) {
				/*@praat
					#
					# result## = owned x## - y#
					#
					y# = { -5, 6, -19 }
					result## = { { 14, -33, 6.25 }, { -33, 17, 9 } } - y#
					assert result## = { { 19, -39, 25.25 }, { -28, 11, 28 } }
				@*/
				x->numericMatrix  -=  y->numericVector;
			} else {
				/*@praat
					#
					# result## = unowned x## - y#
					#
					x## = { { 14, -33, 6.25 }, { -33, 17, 9 } }
					y# = { -5, 6, -19 }
					result## = x## - y#
					assert result## = { { 19, -39, 25.25 }, { -28, 11, 28 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = subtract_MAT (x->numericMatrix, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result## = x## - y
				i.e.
				result## [i, j] = x## [i, j] - y
			*/
			if (x->owned) {
				x->numericMatrix  -=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = subtract_MAT (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		const integer length1 = Melder_length (x->getString());
		const integer length2 = Melder_length (y->getString());
		const integer newlength = length1 - length2;
		autostring32 result;
		if (newlength >= 0 && str32nequ (x->getString() + newlength, y->getString(), length2)) {
			result = autostring32 (newlength);
			str32ncpy (result.get(), x->getString(), newlength);
		} else {
			result = Melder_dup (x->getString());
		}
		x->setString (result.move());
		return;
	}
	Melder_throw (U"Cannot subtract (-) ", y->whichText(), U" from ", x->whichText(), U".");
}
static void do_mul () {
	/*
		result.. = x.. * y..
	*/
	const Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*@praat
				#
				# result = x * y
				#
				x = 5
				y = 6
				result = x * y
				assert result = 30
			@*/
			x->number *= y->number;
			//x->which = Stackel_NUMBER;   // superfluous, as is cleaning up
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x * y#
			*/
			if (y->owned) {
				/*@praat
					#
					# result# = x * owned y#
					#
					result# = 5 * { 11, 13, 31 }   ; numeric vector literals are owned
					assert result# = { 55, 65, 155 }
				@*/
				y->numericVector  *=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = x * unowned y#
					#
					y# = { 17, -11, 29 }
					result# = 30 * y#   ; numeric vector variables are not owned
					assert result# = { 510, -330, 870 }
				@*/
				// x does not have to be cleaned up, because it was a number
				x->numericVector = multiply_VEC (y->numericVector, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x * y##
			*/
			if (y->owned) {
				y->numericMatrix  *=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was a number
				x->numericMatrix = multiply_MAT (y->numericMatrix, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# * y#
				i.e.
				result# [i] = x# [i] * y# [i]
			*/
			const integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny) {
				/*@praat
					#
					# Error: unequal sizes.
					#
					x# = { 11, 13, 17 }
					y# = { 8, 90 }
					asserterror When multiplying vectors, their numbers of elements should be equal, instead of 3 and 2.
					result# = x# * y#
				@*/
				Melder_throw (U"When multiplying vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			}
			if (x -> owned) {
				/*@praat
					#
					# result# = owned x# * y#
					#
					result# = { 11, 13, 17 } * { 44, 56, 67 }   ; owned * owned
					assert result# = { 484, 728, 1139 }
					y# = { 3, 2, 89.5 }
					result# = { 11, 13, 17 } * y#   ; owned * unowned
					assert result# = { 33, 26, 1521.5 }
				@*/
				x->numericVector  *=  y->numericVector;
			} else if (y -> owned) {
				/*@praat
					#
					# result# = unowned x# * owned y#
					#
					x# = { 14, -3, 6.25 }
					result# = x# * { 55, 1, -89 }
					assert result# = { 770, -3, -556.25 }
				@*/
				y->numericVector  *=  x->numericVector;
				// x does not have to be cleaned up, because it was not owned
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = unowned x# * unowned y#
					#
					x# = { 14, -33, 6.25 }
					y# = { -33, 17, 9 }
					result# = x# * y#
					assert result# = { -462, -561, 56.25 }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = multiply_VEC (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x# * y##
				i.e.
				result## [i, j] = x# [i] * y## [i, j]
			*/
			const integer xsize = x->numericVector.size;
			const integer ynrow = y->numericMatrix.nrow;
			Melder_require (ynrow == xsize,
				U"When multiplying a vector with a matrix, the matrix’s number of rows should be equal to the vector’s size, "
				"instead of ", ynrow, U" and ", xsize, U"."
			);
			if (x->owned) {
				/*@praat
					assert { 1, 2, 3 } * { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 1, 2 }, { 6, 8 }, { 15, 18 } }
				@*/
				autoMAT newMatrix = multiply_MAT (x->numericVector, y->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix. releaseToAmbiguousOwner();
			} else {
				/*@praat
					a# = { 1, 2, 3 }
					assert a# * { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 1, 2 }, { 6, 8 }, { 15, 18 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = multiply_MAT (x->numericVector, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# * y
				i.e.
				result# [i] = x# [i] * y
			*/
			if (x->owned) {
				x->numericVector  *=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = multiply_VEC (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x## * y##
				i.e.
				result## [i, j] = x## [i, j] * y## [i, j]
			*/
			const integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			const integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When multiplying matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When multiplying matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  *=  y->numericMatrix;
			} else if (y->owned) {
				y->numericMatrix  *=  x->numericMatrix;
				// x does not have to be cleaned up, because it was not owned
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = multiply_MAT (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result## = x## * y#
				i.e.
				result## [i, j] = x## [i, j] * y# [j]
			*/
			const integer xncol = x->numericMatrix.ncol;
			const integer ysize = y->numericVector.size;
			Melder_require (xncol == ysize,
				U"When multiplying a matrix with a vector, the vector’s size should be equal to the matrix’s number of columns, "
				"instead of ", ysize, U" and ", xncol, U"."
			);
			if (x->owned) {
				x->numericMatrix  *=  y->numericVector;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = multiply_MAT (x->numericMatrix, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result## = x## * y
				i.e.
				result## [i, j] = x## [i, j] * y
			*/
			if (x->owned) {
				x->numericMatrix  *=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = multiply_MAT (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	Melder_throw (U"Cannot multiply (*) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_rdiv () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number / y->number);   // result could be inf (1/0) or NaN (0/0), which is OK
		return;
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			const integer nelem1 = x->numericVector.size, nelem2 = y->numericVector.size;
			if (nelem1 != nelem2)
				Melder_throw (U"When dividing vectors, their numbers of elements should be equal, instead of ", nelem1, U" and ", nelem2, U".");
			autoVEC result = raw_VEC (nelem1);
			for (integer ielem = 1; ielem <= nelem1; ielem ++)
				result [ielem] = x->numericVector [ielem] / y->numericVector [ielem];
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# / y
			*/
			const integer xn = x->numericVector.size;
			autoVEC result = raw_VEC (xn);
			const double yvalue = y->number;
			if (yvalue == 0.0) {
				Melder_throw (U"Cannot divide (/) ", x->whichText(), U" by zero.");
			} else {
				for (integer i = 1; i <= xn; i ++) {
					const double xvalue = x->numericVector [i];
					result [i] = xvalue / yvalue;
				}
			}
			pushNumericVector (result.move());
			return;
		}
	}
	Melder_throw (U"Cannot divide (/) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_idiv () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (floor (x->number / y->number));
		return;
	}
	Melder_throw (U"Cannot divide (“div”) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_mod () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number - floor (x->number / y->number) * y->number);
		return;
	}
	Melder_throw (U"Cannot divide (“mod”) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_minus () {
	const Stackel x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		x->number = - x->number;
	} else if (x->which == Stackel_NUMERIC_VECTOR) {
		if (x->owned) {
			neg_VEC_inout (x->numericVector);
		} else {
			autoVEC result = neg_VEC (x->numericVector);
			x->numericVector = result. releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		if (x->owned) {
			neg_MAT_inout (x->numericMatrix);
		} else {
			autoMAT result = neg_MAT (x->numericMatrix);
			x->numericMatrix = result. releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else {
		Melder_throw (U"Cannot take the opposite (-) of ", x->whichText(), U".");
	}
}
static void do_power () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : pow (x->number, y->number));
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMBER) {
		/*@praat
			assert { 3, 4 } ^ 3 = { 27, 64 }
			assert { 3, -4 } ^ 3 = { 27, -64 }
			assert { -4 } ^ 2.3 = { undefined }
		@*/
		pushNumericVector (power_VEC (x->numericVector, y->number));
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMBER) {
		pushNumericMatrix (power_MAT (x->numericMatrix, y->number));
	} else {
		Melder_throw (U"Cannot exponentiate (^) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_sqr () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number * x->number);
	} else if (x->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			a# = zero# (10)
			a# ~ sum ({ 3, 4 } ^ 2)
			assert sum (a#) = 250
			a# ~ sum ({ col } ^ 2)
			assert sum (a#) = 385
		@*/
		pushNumericVector (power_VEC (x->numericVector, 2.0));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumericMatrix (power_MAT (x->numericMatrix, 2.0));
	} else {
		Melder_throw (U"Cannot take the square (^ 2) of ", x->whichText(), U".");
	}
}
static void do_function_n_n (double (*f) (double)) {
	const Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : f (x->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric argument, not ", x->whichText(), U".");
	}
}
static void do_functionvec_n_n (double (*f) (double)) {
	const Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		const integer n = x->numericVector.size;
		const VEC at = x->numericVector;
		if (x->owned) {
			for (integer i = 1; i <= n; i ++)
				at [i] = f (at [i]);
		} else {
			autoVEC result = raw_VEC (n);
			for (integer i = 1; i <= n; i ++)
				result [i] = f (at [i]);
			x->numericVector = result. releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", x->whichText(), U".");
	}
}
static void do_softmax_VEC () {
	const Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (! x->owned) {
			x->numericVector = copy_VEC (x->numericVector). releaseToAmbiguousOwner();   // TODO: no need to copy
			x->owned = true;
		}
		const integer numberOfElements = x->numericVector.size;
		double maximum = -1e308;
		for (integer i = 1; i <= numberOfElements; i ++) {
			if (x->numericVector [i] > maximum)
				maximum = x->numericVector [i];
		}
		for (integer i = 1; i <= numberOfElements; i ++)
			x->numericVector [i] -= maximum;
		longdouble sum = 0.0;
		for (integer i = 1; i <= numberOfElements; i ++) {
			x->numericVector [i] = exp (x->numericVector [i]);
			sum += x->numericVector [i];
		}
		for (integer i = 1; i <= numberOfElements; i ++)
			x->numericVector [i] /= (double) sum;
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", x->whichText(), U".");
	}
}
static void do_softmaxPerRow_MAT () {
	const Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (! x->owned) {
			x->numericMatrix = copy_MAT (x->numericMatrix). releaseToAmbiguousOwner();   // TODO: no need to copy
			x->owned = true;
		}
		const integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
		for (integer irow = 1; irow <= nrow; irow ++) {
			double maximum = -1e308;
			for (integer icol = 1; icol <= ncol; icol ++) {
				if (x->numericMatrix [irow] [icol] > maximum)
					maximum = x->numericMatrix [irow] [icol];
			}
			for (integer icol = 1; icol <= ncol; icol ++)
				x->numericMatrix [irow] [icol] -= maximum;
			longdouble sum = 0.0;
			for (integer icol = 1; icol <= ncol; icol ++) {
				x->numericMatrix [irow] [icol] = exp (x->numericMatrix [irow] [icol]);
				sum += x->numericMatrix [irow] [icol];
			}
			for (integer icol = 1; icol <= ncol; icol ++)
				x->numericMatrix [irow] [icol] /= (double) sum;
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric matrix argument, not ", x->whichText(), U".");
	}
}

#define DO_NUM_WITH_TENSORS(function, formula, message)  \
static void do_##function () { \
	const Stackel x = pop; \
	if (x->which == Stackel_NUMBER) { \
		const double xvalue = x->number; \
		pushNumber (formula); \
	} else if (x->which == Stackel_NUMERIC_VECTOR) { \
		Melder_throw (U"The function " #function " requires a numeric argument, " \
				"not a vector. Did you mean to use " #function "# instead?"); \
	} else if (x->which == Stackel_NUMERIC_MATRIX) { \
		Melder_throw (U"The function " #function " requires a numeric argument, " \
				"not a matrix. Did you mean to use " #function "## instead?"); \
	} else { \
		Melder_throw (message, x->whichText(), \
				U". The function " #function " requires a numeric argument."); \
	} \
} \
static void do_##function##_VEC () { \
	const Stackel x = topOfStack; \
	if (x->which == Stackel_NUMERIC_VECTOR) { \
		if (x->owned) { \
			const integer numberOfElements = x->numericVector.size; \
			for (integer i = 1; i <= numberOfElements; i ++) { \
				const double xvalue = x->numericVector [i]; \
				x->numericVector [i] = isundef (xvalue) ? undefined : formula; \
			} \
		} else { \
			(void) pop; \
			const integer numberOfElements = x->numericVector.size; \
			autoVEC result = raw_VEC (numberOfElements); \
			for (integer i = 1; i <= numberOfElements; i ++) { \
				const double xvalue = x->numericVector [i]; \
				result [i] = isundef (xvalue) ? undefined : formula; \
			} \
			pushNumericVector (result.move()); \
		} \
	} else { \
		Melder_throw (U"The function " #function "# requires a vector argument, not ", \
				x->whichText(), U"."); \
	} \
} \
static void do_##function##_MAT () { \
	const Stackel x = topOfStack; \
	if (x->which == Stackel_NUMERIC_MATRIX) { \
		if (x->owned) { \
			const integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol; \
			for (integer irow = 1; irow <= nrow; irow ++) { \
				for (integer icol = 1; icol <= ncol; icol ++) { \
					const double xvalue = x->numericMatrix [irow] [icol]; \
					x->numericMatrix [irow] [icol] = isundef (xvalue) ? undefined : formula; \
				} \
			} \
		} else { \
			(void) pop; \
			const integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol; \
			autoMAT result = raw_MAT (nrow, ncol); \
			for (integer irow = 1; irow <= nrow; irow ++) { \
				for (integer icol = 1; icol <= ncol; icol ++) { \
					const double xvalue = x->numericMatrix [irow] [icol]; \
					result [irow] [icol] = isundef (xvalue) ? undefined : formula; \
				} \
			} \
			pushNumericMatrix (result.move()); \
		} \
	} else { \
		Melder_throw (U"The function " #function "## requires a matrix argument, not ", \
				x->whichText(), U"."); \
	} \
}
DO_NUM_WITH_TENSORS (abs, fabs (xvalue), U"Cannot take the absolute value (abs) of ")
DO_NUM_WITH_TENSORS (round, floor (xvalue + 0.5), U"Cannot round ")
DO_NUM_WITH_TENSORS (floor, Melder_roundDown (xvalue), U"Cannot round down (floor) ")
DO_NUM_WITH_TENSORS (ceiling, Melder_roundUp (xvalue), U"Cannot round up (ceiling) ")
DO_NUM_WITH_TENSORS (rectify, xvalue < 0.0 ? 0.0 : xvalue, U"Cannot rectify ")   // NaN-safe
/*@praat
	assert rectify (-1.0) = 0.0
	assert rectify (0.0) = 0.0
	assert rectify (1.0) = 1.0
	assert rectify (undefined) = undefined
	assert rectify# ({ 1.2, -2.3, 4.5, 0, undefined, -44 }) = { 1.2, 0, 4.5, 0, undefined, 0 }
@*/
DO_NUM_WITH_TENSORS (sqrt, sqrt (xvalue), U"Cannot take the square root (sqrt) of ")
/*@praat
	assert sqrt (-1.0) = undefined
	assert sqrt (0.0) = 0.0
	assert sqrt (1.0) = 1.0
	assert sqrt (4.0) = 2.0
	assert sqrt (undefined) = undefined
	assert sqrt# ({ -1.0, 0.0, 1.0, 4.0, undefined }) = { undefined, 0.0, 1.0, 2.0, undefined }
@*/
DO_NUM_WITH_TENSORS (sin, sin (xvalue), U"Cannot take the sine (sin) of ")
DO_NUM_WITH_TENSORS (cos, cos (xvalue), U"Cannot take the cosine (cos) of ")
DO_NUM_WITH_TENSORS (tan, tan (xvalue), U"Cannot take the tangent (tan) of ")
DO_NUM_WITH_TENSORS (arcsin, asin (xvalue), U"Cannot take the arcsine (arcsin) of ")
/*@praat
	assert arcsin (-1.1) = undefined
	assert abs (arcsin (-1.0) - -pi/2) < 1e-17
	assert arcsin (0.0) = 0
	assert abs (arcsin (1.0) - pi/2) < 1e-17
	assert arcsin (1.1) = undefined
	assert arcsin (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (arccos, acos (xvalue), U"Cannot take the arccosine (arccos) of ")
/*@praat
	assert arccos (-1.1) = undefined
	assert abs (arccos (-1.0) - pi) < 1e-17
	assert abs (arccos (0.0) - pi/2) < 1e-17
	assert arccos (1.0) = 0
	assert arccos (1.1) = undefined
	assert arccos (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (arctan, atan (xvalue), U"Cannot take the arctangent (arctan) of ")
DO_NUM_WITH_TENSORS (exp, exp (xvalue), U"Cannot exponentiate (exp) ")
DO_NUM_WITH_TENSORS (sinh, sinh (xvalue), U"Cannot take the hyperbolic sine (sinh) of ")
DO_NUM_WITH_TENSORS (cosh, cosh (xvalue), U"Cannot take the hyperbolic cosine (cosh) of ")
DO_NUM_WITH_TENSORS (tanh, tanh (xvalue), U"Cannot take the hyperbolic tangent (tanh) of ")
DO_NUM_WITH_TENSORS (arcsinh, asinh (xvalue), U"Cannot take the hyperbolic arcsine (arcsinh) of ")
/*@praat
	assert arcsinh (-1.0) < 0
	assert arcsinh (0.0) = 0
	assert arcsinh (1.0) > 0
	assert arcsinh (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (arccosh, acosh (xvalue), U"Cannot take the hyperbolic arccosine (arccosh) of ")
/*@praat
	assert arccosh (1.0) = 0
	assert arccosh (0.9) = undefined
	assert arccosh (0.0) = undefined
	assert arccosh (-10.0) = undefined
	assert arccosh (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (arctanh, atanh (xvalue), U"Cannot take the hyperbolic arctangent (arctanh) of ")
/*@praat
	assert arctanh (-1.0) = undefined
	assert arctanh (0.0) = 0
	assert arctanh (1.0) = undefined
	assert arctanh (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (log2, log (xvalue) * NUMlog2e, U"Cannot take the base-2 logarithm (log2) of ")
/*@praat
	assert log2 (-1.0) = undefined
	assert log2 (0.0) = undefined
	assert log2 (1.0) = 0.0
	assert log2 (2.0) = 1.0
	assert log2 (10.0) > 3.0
	assert log2 (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (ln, log (xvalue), U"Cannot take the natural logarithm (ln) of ")
/*@praat
	assert ln (-1.0) = undefined
	assert ln (0.0) = undefined
	assert ln (1.0) = 0.0
	assert abs (ln (2.0) - 0.693) < 0.001
	assert ln (3.0) > 1.0
	assert ln (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (log10, log10 (xvalue), U"Cannot take the base-10 logarithm (log10) of ")
/*@praat
	assert log10 (-1.0) = undefined
	assert log10 (0.0) = undefined
	assert log10 (1.0) = 0.0
	assert abs (log10 (2.0) - 0.301) < 0.001
	assert log10 (10.0) = 1.0
	assert log10 (11.0) > 1.0
	assert log10 (undefined) = undefined
@*/
DO_NUM_WITH_TENSORS (sigmoid, NUMsigmoid (xvalue), U"Cannot take the sigmoid of ")
/*@praat
	assert sigmoid (0.0) = 0.5
	assert sigmoid (-1000) = 0
	assert arctanh (1000) = undefined
@*/
DO_NUM_WITH_TENSORS (invSigmoid, NUMinvSigmoid (xvalue), U"Cannot take the inverse sigmoid of ")
/*@praat
	assert invSigmoid (0.5) = 0.0
	assert invSigmoid (-1.0) = undefined   ; not a number
	assert invSigmoid (0.0) = undefined   ; minus infinity
	assert invSigmoid (1.0) = undefined   ; plus infinity
	assert invSigmoid (2.0) = undefined   ; not a number
@*/

static void do_sum () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMsum (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMsum (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the sum of ", x->whichText(), U".");
	}
}
static void do_mean () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMmean (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMmean (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the mean of ", x->whichText(), U".");
	}
}
static void do_stdev () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMstdev (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMstdev (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the standard deviation of ", x->whichText(), U".");
	}
}
static void do_center () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMcenterOfGravity (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the center of ", x->whichText(), U".");
	}
}
static void do_randomImax () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMrandomImax (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute a random imax of ", x->whichText(), U".");
	}
}
static void do_function_dd_d (double (*f) (double, double)) {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : f (x->number, y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_VECdd_d (double (*f) (double, double)) {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	const Stackel y = pop, x = pop, a = pop;
	if ((a->which == Stackel_NUMERIC_VECTOR || a->which == Stackel_NUMBER) && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		const integer numberOfElements = ( a->which == Stackel_NUMBER ? Melder_iround (a->number) : a->numericVector.size );
		autoVEC newData = raw_VEC (numberOfElements);
		for (integer ielem = 1; ielem <= numberOfElements; ielem ++)
			newData [ielem] = f (x->number, y->number);
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires either three numeric arguments, or one vector argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_MATdd_d (double (*f) (double, double)) {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 3) {
		const Stackel y = pop, x = pop, model = pop;
		if (model->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
			const integer numberOfRows = model->numericMatrix.nrow;
			const integer numberOfColumns = model->numericMatrix.ncol;
			autoMAT newData = raw_MAT (numberOfRows, numberOfColumns);
			for (integer irow = 1; irow <= numberOfRows; irow ++)
				for (integer icol = 1; icol <= numberOfColumns; icol ++)
					newData [irow] [icol] = f (x->number, y->number);
			pushNumericMatrix (newData.move());
		} else {
			Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
				U" requires one matrix argument and two numeric arguments, not ",
				model->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
		}
	} else if (narg->number == 4) {
		const Stackel y = pop, x = pop, ncol = pop, nrow = pop;
		if (nrow->which == Stackel_NUMBER && ncol->which == Stackel_NUMBER && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
			const integer numberOfRows = Melder_iround (nrow->number);
			const integer numberOfColumns = Melder_iround (ncol->number);
			autoMAT newData = raw_MAT (numberOfRows, numberOfColumns);
			for (integer irow = 1; irow <= numberOfRows; irow ++)
				for (integer icol = 1; icol <= numberOfColumns; icol ++)
					newData [irow] [icol] = f (x->number, y->number);
			pushNumericMatrix (newData.move());
		} else {
			Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
				U" requires four numeric arguments, not ",
				nrow->whichText(), U", ", ncol->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
		}
	} else
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three or four arguments.");
}

static void do_function_VECll_l (integer (*f) (integer, integer)) {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg-> number == 3,
		U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	const Stackel y = pop, x = pop, a = pop;
	if ((a->which == Stackel_NUMERIC_VECTOR || a->which == Stackel_NUMBER) && x->which == Stackel_NUMBER) {
		const integer numberOfElements = ( a->which == Stackel_NUMBER ? Melder_iround (a->number) : a->numericVector.size );
		autoVEC newData = raw_VEC (numberOfElements);
		for (integer ielem = 1; ielem <= numberOfElements; ielem ++)
			newData [ielem] = f (Melder_iround (x->number), Melder_iround (y->number));
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires either three numeric arguments, or one vector argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_MATll_l (integer (*f) (integer, integer)) {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	const Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		const integer numberOfRows = a->numericMatrix.nrow;
		const integer numberOfColumns = a->numericMatrix.ncol;
		autoMAT newData = raw_MAT (numberOfRows, numberOfColumns);
		for (integer irow = 1; irow <= numberOfRows; irow ++)
			for (integer icol = 1; icol <= numberOfColumns; icol ++)
				newData [irow] [icol] = f (Melder_iround (x->number), Melder_iround (y->number));
		pushNumericMatrix (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one matrix argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_dl_d (double (*f) (double, integer)) {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (x->number, Melder_iround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ld_d (double (*f) (integer, double)) {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (Melder_iround (x->number), y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ll_l (integer (*f) (integer, integer)) {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (Melder_iround (x->number), Melder_iround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_objects_are_identical () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		const integer id1 = Melder_iround (x->number), id2 = Melder_iround (y->number);
		integer i = theCurrentPraatObjects -> n;
		while (i > 0 && id1 != theCurrentPraatObjects -> list [i]. id)
			i --;
		if (i == 0)
			Melder_throw (U"Object #", id1, U" does not exist in function objectsAreIdentical.");
		const Daata object1 = (Daata) theCurrentPraatObjects -> list [i]. object;
		i = theCurrentPraatObjects -> n;
		while (i > 0 && id2 != theCurrentPraatObjects -> list [i]. id)
			i --;
		if (i == 0)
			Melder_throw (U"Object #", id2, U" does not exist in function objectsAreIdentical.");
		const Daata object2 = (Daata) theCurrentPraatObjects -> list [i]. object;
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : Data_equal (object1, object2));
	} else {
		Melder_throw (U"The function objectsAreIdentical requires two numeric arguments (object IDs), not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ddd_d (double (*f) (double, double, double)) {
	const Stackel z = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER && z->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) || isundef (z->number) ? undefined :
			f (x->number, y->number, z->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires three numeric arguments, not ", x->whichText(), U", ",
			y->whichText(), U", and ", z->whichText(), U".");
	}
}
static void do_do () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function “do” requires at least one argument, namely a menu command.");
	const integer numberOfArguments = Melder_iround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (integer iarg = numberOfArguments; iarg >= 0; iarg --) {
		const Stackel arg = pop;
		stack [iarg] = std::move (*arg);
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function “do” should be a string, namely a menu command, and not ", stack [0]. whichText(), U".");
	conststring32 command = stack [0]. getString();
	if (theCurrentPraatObjects == & theForegroundPraatObjects && theInterpreter -> hasDynamicEnvironmentEditor()) {
		autoMelderString valueString;
		MelderString_appendCharacter (& valueString, 1);   // TODO: check whether this is needed at all, or is just MelderString_empty enough?
		autoMelderDivertInfo divert (& valueString);
		autostring32 command2 = Melder_dup (command);   // allow the menu command to reuse the stack (?)
		Editor_doMenuCommand (theInterpreter -> optionalDynamicEnvironmentEditor(), command2.get(), numberOfArguments, & stack [0], nullptr, theInterpreter);
		pushNumber (Melder_atof (valueString.string));
		return;
	} else if (! praat_commandsWithExternalSideEffectsAreAllowed () &&
		(str32nequ (command, U"Save ", 5) || str32nequ (command, U"Write ", 6) || str32nequ (command, U"Append to ", 10) || str32nequ (command, U"Quit", 4)))
	{
		Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
	} else {
		autoMelderString valueString;
		MelderString_appendCharacter (& valueString, 1);   // a semaphor to check whether praat_doAction or praat_doMenuCommand wrote anything with MelderInfo
		autoMelderDivertInfo divert (& valueString);
		autostring32 command2 = Melder_dup (command);   // allow the menu command to reuse the stack (?)
		if (! praat_doAction (command2.get(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.get(), numberOfArguments, & stack [0], theInterpreter))
		{
			Melder_throw (U"Command “", command, U"” not available for current selection.");
		}
		//praat_updateSelection ();
		double value = undefined;
		if (valueString.string [0] == 1) {   // nothing written with MelderInfo by praat_doAction or praat_doMenuCommand? then the return value is the ID of the selected object
			integer IOBJECT, result = 0, found = 0;
			WHERE (SELECTED) {
				result = IOBJECT;
				found += 1;
			}
			if (found == 1)
				value = theCurrentPraatObjects -> list [result]. id;
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
		pushNumber (value);
		return;
	}
	praat_updateSelection ();   // BUG: superfluous? flickering?
	pushNumber (1);
}
static void do_evaluate () {
	const Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		double result;
		Interpreter_numericExpression (theInterpreter, expression->getString(), & result);
		pushNumber (result);
	} else Melder_throw (U"The argument of the function “evaluate” should be a string with a numeric expression, not ", expression->whichText());
}
static void do_evaluate_nocheck () {
	const Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		try {
			double result;
			Interpreter_numericExpression (theInterpreter, expression->getString(), & result);
			pushNumber (result);
		} catch (MelderError) {
			Melder_clearError ();
			pushNumber (undefined);
		}
	} else Melder_throw (U"The argument of the function “evaluate_nocheck” should be a string with a numeric expression, not ", expression->whichText());
}
static void do_evaluate_STR () {
	const Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		autostring32 result = Interpreter_stringExpression (theInterpreter, expression->getString());
		pushString (result.move());
	} else Melder_throw (U"The argument of the function “evaluate$” should be a string with a string expression, not ", expression->whichText());
}
static void do_evaluate_nocheck_STR () {
	const Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		try {
			autostring32 result = Interpreter_stringExpression (theInterpreter, expression->getString());
			pushString (result.move());
		} catch (MelderError) {
			Melder_clearError ();
			pushString (Melder_dup (U""));
		}
	} else Melder_throw (U"The argument of the function “evaluate_nocheck$” should be a string with a string expression, not ", expression->whichText());
}
static void do_do_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function “do$” requires at least one argument, namely a menu command.");
	const integer numberOfArguments = Melder_iround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (integer iarg = numberOfArguments; iarg >= 0; iarg --) {
		const Stackel arg = pop;
		stack [iarg] = std::move (*arg);
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function “do$” should be a string, namely a menu command, and not ", stack [0]. whichText(), U".");
	conststring32 command = stack [0]. getString();
	if (theCurrentPraatObjects == & theForegroundPraatObjects && theInterpreter -> hasDynamicEnvironmentEditor()) {
		static MelderString info;
		MelderString_empty (& info);
		autoMelderDivertInfo divert (& info);
		autostring32 command2 = Melder_dup (command);
		Editor_doMenuCommand (theInterpreter -> optionalDynamicEnvironmentEditor(), command2.get(), numberOfArguments, & stack [0], nullptr, theInterpreter);
		pushString (Melder_dup (info.string));
		return;
	} else if (! praat_commandsWithExternalSideEffectsAreAllowed () &&
		(str32nequ (command, U"Save ", 5) || str32nequ (command, U"Write ", 6) || str32nequ (command, U"Append to ", 10) || str32nequ (command, U"Quit", 4)))
	{
		Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
	} else {
		static MelderString info;
		MelderString_empty (& info);
		autoMelderDivertInfo divert (& info);
		autostring32 command2 = Melder_dup (command);
		if (! praat_doAction (command2.get(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.get(), numberOfArguments, & stack [0], theInterpreter))
		{
			Melder_throw (U"Command “", command, U"” not available for current selection.");
		}
		praat_updateSelection ();
		pushString (Melder_dup (info.string));
		return;
	}
	praat_updateSelection ();   // BUG: superfluous? flickering?
	pushString (Melder_dup (U""));
}
static void shared_do_writeInfo (integer numberOfArguments) {
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER) {
			MelderInfo_write (arg->number);
		} else if (arg->which == Stackel_STRING) {
			MelderInfo_write (arg->getString());
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++)
				MelderInfo_write (arg->numericVector [i],
						i == arg->numericVector.size ? U"" : U" ");
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++)
					MelderInfo_write (arg->numericMatrix [irow] [icol],
							icol == arg->numericMatrix.ncol ? U"" : U" ");
				MelderInfo_write (irow == arg->numericMatrix.nrow ? U"" : U"\n");
			}
		} else if (arg->which == Stackel_STRING_ARRAY) {
			for (integer i = 1; i <= arg->stringArray.size; i ++)
				MelderInfo_write (arg->stringArray [i],
						i == arg->stringArray.size ? U"" : U" ");
		}
	}
}
static void do_writeInfo () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	MelderInfo_open ();
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_writeInfoLine () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	MelderInfo_open ();
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_write (U"\n");
	MelderInfo_drain ();
	pushNumber (1);
}

static void do_appendInfo () {
	MELDER_ASSERT_NONREENTRANT
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_appendInfoLine () {
	MELDER_ASSERT_NONREENTRANT
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_write (U"\n");
	MelderInfo_drain ();
	pushNumber (1);
}
static void shared_do_writeFile (autoMelderString *text, integer numberOfArguments) {
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER) {
			MelderString_append (text, arg->number);
		} else if (arg->which == Stackel_STRING) {
			MelderString_append (text, arg->getString());
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++)
				MelderString_append (text, arg->numericVector [i],
						i == arg->numericVector.size ? U"" : U" ");
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++)
					MelderString_append (text, arg->numericMatrix [irow] [icol],
							icol == arg->numericMatrix.ncol ? U"" : U" ");
				MelderString_append (text, irow == arg->numericMatrix.nrow ? U"" : U"\n");
			}
		} else if (arg->which == Stackel_STRING_ARRAY) {
			for (integer i = 1; i <= arg->stringArray.size; i ++)
				MelderString_append (text, arg->stringArray [i],
						i == arg->stringArray.size ? U"" : U" ");
		}
	}
}
static void do_writeFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “writeFile” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument of “writeFile” should be a string (a file name), not ", fileName->whichText(), U".");
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	structMelderFile file { };
	Melder_relativePathToFile (fileName->getString(), & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_writeFileLine () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “writeFileLine” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument of “writeFileLine” should be a string (a file name), not ", fileName->whichText(), U".");
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	MelderString_appendCharacter (& text, U'\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName->getString(), & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_appendFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “appendFile” is not available inside manuals.");
	const Stackel elNumberOfArguments = pop;
	Melder_assert (elNumberOfArguments->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (elNumberOfArguments->number);
	stackPointer -= numberOfArguments;
	const Stackel elFileName = & theStack [stackPointer + 1];
	Melder_require (elFileName->which == Stackel_STRING,
		U"The first argument of “appendFile” should be a string (a file name), not ", elFileName->whichText(), U".");
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	structMelderFile file { };
	Melder_relativePathToFile (elFileName->getString(), & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_appendFileLine () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “appendFileLine” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument of “appendFileLine” should be a string (a file name), not ", fileName->whichText(), U".");
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	MelderString_appendCharacter (& text, '\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName->getString(), & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_pauseScript () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “pauseScript” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	if (! theCurrentPraatApplication -> batch) {   // in batch we ignore pause statements
		autoMelderString buffer;
		for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
			const Stackel arg = & theStack [stackPointer + iarg];
			if (arg->which == Stackel_NUMBER) {
				MelderString_append (& buffer, arg->number);
			} else if (arg->which == Stackel_STRING) {
				MelderString_append (& buffer, arg->getString());
			} else if (arg->which == Stackel_NUMERIC_VECTOR) {
				for (integer i = 1; i <= arg->numericVector.size; i ++)
					MelderString_append (& buffer, arg->numericVector [i],
							i == arg->numericVector.size ? U"" : U" ");
			} else if (arg->which == Stackel_NUMERIC_MATRIX) {
				for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
					for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++)
						MelderString_append (& buffer, arg->numericMatrix [irow] [icol],
								icol == arg->numericMatrix.ncol ? U"" : U" ");
					MelderString_append (& buffer, irow == arg->numericMatrix.nrow ? U"" : U"\n");
				}
			} else if (arg->which == Stackel_STRING_ARRAY) {
				for (integer i = 1; i <= arg->stringArray.size; i ++)
					MelderString_append (& buffer, arg->stringArray [i],
							i == arg->stringArray.size ? U"" : U" ");
			}
		}
		const Editor optionalPauseWindowOwningEditor = theInterpreter -> optionalDynamicEnvironmentEditor();
		const GuiWindow parentShell = ( optionalPauseWindowOwningEditor ? optionalPauseWindowOwningEditor -> windowForm : theCurrentPraatApplication -> topShell );
		UiPause_pauseScript (parentShell, optionalPauseWindowOwningEditor, theInterpreter, numberOfArguments == 0 ? U"..." : buffer.string);
	}
	pushNumber (1);
}
static void do_exitScript () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER) {
			Melder_appendError_noLine (arg->number);
		} else if (arg->which == Stackel_STRING) {
			Melder_appendError_noLine (arg->getString());
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++) {
				Melder_appendError_noLine (arg->numericVector [i]);
				Melder_appendError_noLine (i == arg->numericVector.size ? U"" : U" ");
			}
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++) {
					Melder_appendError_noLine (arg->numericMatrix [irow] [icol]);
					Melder_appendError_noLine (icol == arg->numericMatrix.ncol ? U"" : U" ");
				}
				Melder_appendError_noLine (irow == arg->numericMatrix.nrow ? U"" : U"\n");
			}
		} else if (arg->which == Stackel_STRING_ARRAY) {
			for (integer i = 1; i <= arg->stringArray.size; i ++) {
				Melder_appendError_noLine (arg->stringArray [i]);
				Melder_appendError_noLine (i == arg->stringArray.size ? U"" : U" ");
			}
		}
	}
	Melder_throw (U"\nScript exited.");
	pushNumber (1);
}
static void do_runScript () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	if (numberOfArguments < 1)
		Melder_throw (U"The function “runScript” requires at least one argument, namely the file name.");
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument to “runScript” should be a string (the file name), not ", fileName->whichText());
	theLevel += 1;
	if (theLevel > MAXIMUM_NUMBER_OF_LEVELS) {   // TODO: remove
		theLevel -= 1;
		//Melder_throw (U"Cannot call runScript() more than ", MAXIMUM_NUMBER_OF_LEVELS, U" levels deep.");
	}
	try {
		const Editor optionalNewInterpreterOwningWindow = theInterpreter -> optionalDynamicEnvironmentEditor();
		Melder_assert (theInterpreter -> owningInterpreterStack);
		praat_runScript (theInterpreter -> owningInterpreterStack, fileName->getString(), numberOfArguments - 1, & theStack [stackPointer + 1], optionalNewInterpreterOwningWindow);
		theLevel -= 1;
	} catch (MelderError) {
		theLevel -= 1;
		throw;
	}
	pushNumber (1);
}
static void do_runScriptWithForm () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	if (numberOfArguments != 1)
		Melder_throw (U"The function “runScriptWithForm” requires precisely one argument, namely the file name.");
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument to “runScript” should be a string (the file name), not ", fileName->whichText());
	theLevel += 1;
	if (theLevel > MAXIMUM_NUMBER_OF_LEVELS) {
		theLevel -= 1;
		Melder_throw (U"Cannot call runScriptWithForm() more than ", MAXIMUM_NUMBER_OF_LEVELS, U" levels deep.");
	}
	try {
		praat_runScriptWithForm (fileName->getString());
		theLevel -= 1;
	} catch (MelderError) {
		theLevel -= 1;
		throw;
	}
	pushNumber (1);
}
static void do_runNotebook () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	if (numberOfArguments < 1)
		Melder_throw (U"The function “runNotebook” requires at least one argument, namely the file name.");
	stackPointer -= numberOfArguments;
	const Stackel fileName = & theStack [stackPointer + 1];
	Melder_require (fileName->which == Stackel_STRING,
		U"The first argument to “runNotebook” should be a string (the file name), not ", fileName->whichText());
	theLevel += 1;
	if (theLevel > MAXIMUM_NUMBER_OF_LEVELS) {
		theLevel -= 1;
		Melder_throw (U"Cannot call runNotebook() more than ", MAXIMUM_NUMBER_OF_LEVELS, U" levels deep.");
	}
	try {
		const Editor optionalNewInterpreterOwningWindow = theInterpreter -> optionalDynamicEnvironmentEditor();
		Melder_assert (theInterpreter -> owningInterpreterStack);
		praat_runNotebook (theInterpreter -> owningInterpreterStack, fileName->getString(), numberOfArguments - 1, & theStack [stackPointer + 1], optionalNewInterpreterOwningWindow);
		theLevel -= 1;
	} catch (MelderError) {
		theLevel -= 1;
		throw;
	}
	pushNumber (1);
}
static void do_runSystem () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “runSystem” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	autoMelderString text;
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->getString());
	}
	try {
		Melder_runSystem (text.string);
	} catch (MelderError) {
		Melder_throw (U"System command <<", text.string, U">> returned error status;\n"
			U"if you want to ignore this, use `runSystem_nocheck` instead of `runSystem`.");
	}
	pushNumber (1);
}
static void do_runSystem_STR () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “runSystem$” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	autoMelderString text;
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->getString());
	}
	autostring32 result;
	try {
		result = runSystem_STR (text.string);
	} catch (MelderError) {
		Melder_throw (U"System command <<", text.string, U">> returned error status;\n"
			U"if you want to ignore this, use `runSystem_nocheck$` instead of `runSystem$`.");
	}
	pushString (result.move());
}
static void do_runSystem_nocheck () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “runSystem” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	autoMelderString text;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->getString());
	}
	try {
		Melder_runSystem (text.string);
	} catch (MelderError) {
		Melder_clearError ();
	}
	pushNumber (1);
}
static void do_runSubprocess () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “runSubprocess” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	const Stackel commandFile = & theStack [stackPointer + 1];
	Melder_require (commandFile->which == Stackel_STRING,
		U"The first argument to “runSubprocess” should be a command name.");
	autoSTRVEC arguments (numberOfArguments - 1);
	for (int iarg = 1; iarg < numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + 1 + iarg];
		if (arg->which == Stackel_NUMBER)
			arguments [iarg] = Melder_dup (Melder_double (arg->number));
		else if (arg->which == Stackel_STRING)
			arguments [iarg] = Melder_dup (arg->getString());
	}
	try {
		Melder_runSubprocess (commandFile->getString(), numberOfArguments - 1, arguments.peek2());
	} catch (MelderError) {
		Melder_throw (U"Command “", commandFile->getString(), U"” returned error status.");
	}
	pushNumber (1);
}
static void do_runSubprocess_STR () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “runSubprocess$” is not available inside manuals.");
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	const Stackel commandFile = & theStack [stackPointer + 1];
	Melder_require (commandFile->which == Stackel_STRING,
		U"The first argument to “runSubprocess$” should be a command name.");
	autoSTRVEC arguments (numberOfArguments - 1);
	for (int iarg = 1; iarg < numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + 1 + iarg];
		if (arg->which == Stackel_NUMBER)
			arguments [iarg] = Melder_dup (Melder_double (arg->number));
		else if (arg->which == Stackel_STRING)
			arguments [iarg] = Melder_dup (arg->getString());
	}
	autostring32 result;
	try {
		result = runSubprocess_STR (commandFile->getString(), numberOfArguments - 1, arguments.peek2());
	} catch (MelderError) {
		Melder_throw (U"Command “", commandFile->getString(), U"” returned error status.");
	}
	pushString (result.move());
}
static void do_min () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “min” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		/*@praat
			assert min (5, 6, 1, 7) = 1
			assert min (undefined, 6, 1, 7) = undefined
			assert min (5, undefined, 1, 7) = undefined
			assert min (5, 6, undefined, 7) = undefined
			assert min (5, 6, 1, undefined) = undefined
			assert min (undefined, undefined) = undefined
			assert min (undefined) = undefined
			assert min (5) = 5
		@*/
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “min” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmin_u (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			assert min ({ 5, 6, 1, 7 }) = 1
			assert min ({ undefined, 6, 1, 7 }) = undefined
			assert min ({ 5, undefined, 1, 7 }) = undefined
			assert min ({ 5, 6, undefined, 7 }) = undefined
			assert min ({ 5, 6, 1, undefined }) = undefined
			assert min ({ undefined, undefined }) = undefined
			assert min ({ undefined }) = undefined
			assert min (zero# (0)) = undefined
			assert min ({ }) = undefined
		@*/
		Melder_require (n->number == 1,
			U"The function “min” requires exactly one vector argument.");
		pushNumber (NUMmin_u (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the minimum of ", last->whichText(), U".");
	}
}
static void do_min_e () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “min_e” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		/*@praat
			assert min_e (5, 6, 1, 7) = 1
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e (undefined, 6, 1, 7)
			asserterror cannot determine the minimum of a vector: element 2 is undefined.
			pos = min_e (5, undefined, 1, 7)
			asserterror cannot determine the minimum of a vector: element 3 is undefined.
			pos = min_e (5, 6, undefined, 7)
			asserterror cannot determine the minimum of a vector: element 4 is undefined.
			pos = min_e (5, 6, 1, undefined)
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e (undefined, undefined)
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e (undefined)
			assert min_e (5) = 5
		@*/
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “min_e” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmin_e (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			assert min_e ({ 5, 6, 1, 7 }) = 1
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e ({ undefined, 6, 1, 7 })
			asserterror cannot determine the minimum of a vector: element 2 is undefined.
			pos = min_e ({ 5, undefined, 1, 7 })
			asserterror cannot determine the minimum of a vector: element 3 is undefined.
			pos = min_e ({ 5, 6, undefined, 7 })
			asserterror cannot determine the minimum of a vector: element 4 is undefined.
			pos = min_e ({ 5, 6, 1, undefined })
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e ({ undefined, undefined })
			asserterror cannot determine the minimum of a vector: element 1 is undefined.
			pos = min_e ({ undefined })
			asserterror cannot determine the minimum of an empty vector.
			pos = min_e (zero# (0))
			asserterror cannot determine the minimum of an empty vector.
			pos = min_e ({})
		@*/
		Melder_require (n->number == 1,
			U"The function “min_e” requires exactly one vector argument.");
		pushNumber (NUMmin_e (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the minimum of ", last->whichText(), U".");
	}
}
static void do_min_removeUndefined () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “min_removeUndefined” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		/*@praat
			assert min_removeUndefined (5, 6, 1, 7) = 1
			assert min_removeUndefined (undefined, 6, 1, 7) = 1
			assert min_removeUndefined (5, undefined, 1, 7) = 1
			assert min_removeUndefined (5, 6, undefined, 7) = 5
			assert min_removeUndefined (5, 6, 1, undefined) = 1
			assert min_removeUndefined (undefined, undefined) = undefined
			assert min_removeUndefined (undefined) = undefined
			assert min_removeUndefined (5) = 5
		@*/
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “min_removeUndefined” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmin_removeUndefined_u (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			assert min_removeUndefined ({ 5, 6, 1, 7 }) = 1
			assert min_removeUndefined ({ undefined, 6, 1, 7 }) = 1
			assert min_removeUndefined ({ 5, undefined, 1, 7 }) = 1
			assert min_removeUndefined ({ 5, 6, undefined, 7 }) = 5
			assert min_removeUndefined ({ 5, 6, 1, undefined }) = 1
			assert min_removeUndefined ({ undefined, undefined }) = undefined
			assert min_removeUndefined ({ undefined }) = undefined
			assert min_removeUndefined (zero# (0)) = undefined
			assert min_removeUndefined ({ }) = undefined
		@*/
		Melder_require (n->number == 1,
			U"The function “min_removeUndefined” requires exactly one vector argument.");
		pushNumber (NUMmin_removeUndefined_u (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the minimum of ", last->whichText(), U".");
	}
}
static void do_max () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “max” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “max_removeUndefined” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmax_u (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “max” requires exactly one vector argument.");
		pushNumber (NUMmax_u (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the maximum of ", last->whichText(), U".");
	}
}
static void do_max_e () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “max_e” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “max_e” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmax_e (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “max_e” requires exactly one vector argument.");
		pushNumber (NUMmax_e (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the maximum of ", last->whichText(), U".");
	}
}
static void do_max_removeUndefined () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “max_removeUndefined” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		integer size = Melder_iround (n->number);
		autoVEC numericVector = raw_VEC (size);
		numericVector [size] = last->number;
		for (integer i = size - 1; i > 0; i --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMBER,
				U"The function “max_removeUndefined” cannot mix a numeric argument with ", element->whichText(), U".");
			numericVector [i] = element->number;
		}
		pushNumber (NUMmax_removeUndefined_u (numericVector.get()));
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “max_removeUndefined” requires exactly one vector argument.");
		pushNumber (NUMmax_removeUndefined_u (last->numericVector));
	} else {
		Melder_throw (U"Cannot compute the maximum of ", last->whichText(), U".");
	}
}
static void do_imin () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imin” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double minimum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imin” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (minimum) || isundef (previous->number)) {
				minimum = undefined;
				result = undefined;
			} else if (previous->number < minimum) {
				minimum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imin” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double minimum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] < minimum) {
				result = i;
				minimum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imin of ", last->whichText(), U".");
	}
}
static void do_imin_e () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imin_e” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double minimum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imin_e” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (minimum) || isundef (previous->number)) {
				minimum = undefined;
				result = undefined;
			} else if (previous->number < minimum) {
				minimum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imin_e” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double minimum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] < minimum) {
				result = i;
				minimum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imin of ", last->whichText(), U".");
	}
}
static void do_imin_removeUndefined () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imin_removeUndefined” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double minimum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imin_removeUndefined” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (minimum) || isundef (previous->number)) {
				minimum = undefined;
				result = undefined;
			} else if (previous->number < minimum) {
				minimum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imin_removeUndefined” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double minimum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] < minimum) {
				result = i;
				minimum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imin of ", last->whichText(), U".");
	}
}
static void do_imax () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imax” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double maximum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			const Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imax” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (maximum) || isundef (previous->number)) {
				maximum = undefined;
				result = undefined;
			} else if (previous->number > maximum) {
				maximum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imax” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double maximum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] > maximum) {
				result = i;
				maximum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imax of ", last->whichText(), U".");
	}
}
static void do_imax_e () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imax_e” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double maximum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			const Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imax_e” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (maximum) || isundef (previous->number)) {
				maximum = undefined;
				result = undefined;
			} else if (previous->number > maximum) {
				maximum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imax_e” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double maximum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] > maximum) {
				result = i;
				maximum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imax of ", last->whichText(), U".");
	}
}
static void do_imax_removeUndefined () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number >= 1,
		U"The function “imax_removeUndefined” requires at least one argument.");
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double maximum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			const Stackel previous = pop;
			Melder_require (previous->which == Stackel_NUMBER,
				U"The function “imax_removeUndefined” cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (maximum) || isundef (previous->number)) {
				maximum = undefined;
				result = undefined;
			} else if (previous->number > maximum) {
				maximum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (n->number == 1,
			U"The function “imax_removeUndefined” requires exactly one vector argument.");
		const integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double maximum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] > maximum) {
				result = i;
				maximum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compute the imax of ", last->whichText(), U".");
	}
}
static void do_quantile () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number == 2,
		U"The function “quantile” requires two arguments, not ", n->number, U".");
	const Stackel quantile = pop;
	Melder_require (quantile->which == Stackel_NUMBER,
		U"The second argument to “quantile” should be a number, not ", quantile->which, U".");
	const Stackel vec = pop;
	Melder_require (vec->which == Stackel_NUMERIC_VECTOR,
		U"The first argument to “quantile” should be a numeric vector, not ", vec->which, U".");
	pushNumber (NUMquantile (vec->numericVector, quantile->number));
}
static void do_norm () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	Melder_require (n->number == 1 || n->number == 2,
		U"The function “norm” requires one or two arguments.");
	double powerNumber = 2.0;
	if (n->number == 2) {
		const Stackel power = pop;
		if (power->which != Stackel_NUMBER)
			Melder_throw (U"The second argument to “norm” should be a number, not ", power->whichText(), U".");
		powerNumber = power->number;
	}
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMnorm (x->numericVector, powerNumber));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMnorm (x->numericMatrix, powerNumber));
	} else {
		Melder_throw (U"Cannot compute the norm of ", x->whichText(), U".");
	}
}
static void do_zero_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer rank = Melder_iround (narg->number);
	Melder_require (rank >= 1,
		U"The function “zero#” requires an argument.");
	Melder_require (rank <= 1,
		U"The function “zero#” cannot have more than one argument (consider using zero##).");
	const Stackel nelem = pop;
	Melder_require (nelem->which == Stackel_NUMBER,
		U"In the function “zero#”, the number of elements should be a number, not ", nelem->whichText(), U".");
	const double numberOfElements = nelem->number;
	Melder_require (isdefined (numberOfElements),
		U"In the function “zero#”, the number of elements is undefined.");
	Melder_require (numberOfElements >= 0.0,
		U"In the function “zero#”, the number of elements should not be negative.");
	pushNumericVector (zero_VEC (Melder_iround (numberOfElements)));
}
static void do_zero_MAT () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer rank = Melder_iround (narg->number);
	Melder_require (rank == 2,
		U"The function “zero##” requires two arguments.");
	const Stackel ncol = pop;
	Melder_require (ncol->which == Stackel_NUMBER,
		U"In the function “zero##”, the number of columns should be a number, not ", ncol->whichText(), U".");
	const double numberOfColumns = ncol->number;
	const Stackel nrow = pop;
	Melder_require (nrow->which == Stackel_NUMBER,
		U"In the function “zero##”, the number of rows should be a number, not ", nrow->whichText(), U".");
	const double numberOfRows = nrow->number;
	Melder_require (isdefined (numberOfRows),
		U"In the function “zero##”, the number of rows is undefined.");
	Melder_require (isdefined (numberOfColumns),
		U"In the function “zero##”, the number of columns is undefined.");
	Melder_require (numberOfRows >= 0.0,
		U"In the function “zero##”, the number of rows should not be negative.");
	Melder_require (numberOfColumns >= 0.0,
		U"In the function “zero##”, the number of columns should not be negative.");
	autoMAT result = zero_MAT (Melder_iround (numberOfRows), Melder_iround (numberOfColumns));
	pushNumericMatrix (result.move());
}
static void do_linear_VEC () {
	const Stackel stackel_narg = pop;
	Melder_assert (stackel_narg->which == Stackel_NUMBER);
	const integer narg = Melder_iround (stackel_narg->number);
	Melder_require (narg == 3 || narg == 4,
		U"The function “linear#” requires three or four arguments.");
	bool excludeEdges = false;   // default
	if (narg == 4) {
		Stackel stack_excludeEdges = pop;
		if (stack_excludeEdges->which != Stackel_NUMBER)
			Melder_throw (U"In the function “linear#”, the edge exclusion flag (fourth argument) should be a number, not ", stack_excludeEdges->whichText(), U".");
		excludeEdges = Melder_iround (stack_excludeEdges->number);
	}
	const Stackel stack_numberOfSteps = pop, stack_maximum = pop, stack_minimum = pop;
	Melder_require (stack_minimum -> which == Stackel_NUMBER,
		U"In the function “linear#”, the minimum (first argument) should be a number, not ", stack_minimum->whichText(), U".");
	const double minimum = stack_minimum->number;
	Melder_require (isdefined (minimum),
		U"Undefined minimum in the function “linear#” (first argument).");
	Melder_require (stack_maximum->which == Stackel_NUMBER,
		U"In the function “linear#”, the maximum (second argument) should be a number, not ", stack_maximum->whichText(), U".");
	const double maximum = stack_maximum->number;
	Melder_require (isdefined (maximum),
		U"Undefined maximum in the function “linear#” (second argument).");
	Melder_require (maximum >= minimum,
		U"Maximum (", maximum, U") smaller than minimum (", minimum, U") in function “linear#”.");
	Melder_require (stack_numberOfSteps -> which == Stackel_NUMBER,
		U"In the function “linear#”, the number of steps (third argument) should be a number, not ", stack_numberOfSteps->whichText(), U".");
	Melder_require (isdefined (stack_numberOfSteps->number),
		U"Undefined number of steps in the function “linear#” (third argument).");
	const integer numberOfSteps = Melder_iround (stack_numberOfSteps->number);
	Melder_require (numberOfSteps > 0,
		U"In the function “linear#”, the number of steps (third argument) should be positive, not ", numberOfSteps, U".");
	autoVEC result = raw_VEC (numberOfSteps);
	for (integer ielem = 1; ielem <= numberOfSteps; ielem ++) {
		result [ielem] = excludeEdges ?
			minimum + (ielem - 0.5) * (maximum - minimum) / numberOfSteps :
			minimum + (ielem - 1) * (maximum - minimum) / (numberOfSteps - 1);
	}
	if (! excludeEdges)
		result [numberOfSteps] = maximum;   // remove rounding problems
	pushNumericVector (result.move());
}
static void do_to_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function to#() requires one argument.");
	const Stackel to = pop;
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “to#”, the argument should be a number, not ", to->whichText(), U".");
	autoVEC result = to_VEC (to->number);
	pushNumericVector (result.move());
}
static void do_from_to_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 2,
		U"The function “from_to#” requires two arguments.");
	const Stackel to = pop, from = pop;
	Melder_require (from->which == Stackel_NUMBER,
		U"In the function “from_to#”, the first argument should be a number, not ", from->whichText(), U".");
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “from_to#”, the second argument should be a number, not ", to->whichText(), U".");
	autoVEC result = from_to_VEC (from->number, to->number);
	pushNumericVector (result.move());
}
static void do_from_to_by_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function “from_to_by#” requires three arguments.");
	const Stackel by = pop, to = pop, from = pop;
	Melder_require (from->which == Stackel_NUMBER,
		U"In the function “from_to_by#”, the first argument should be a number, not ", from->whichText(), U".");
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “from_to_by#”, the second argument should be a number, not ", to->whichText(), U".");
	Melder_require (by->which == Stackel_NUMBER,
		U"In the function “from_to_by#”, the third argument should be a number (the step), not ", by->whichText(), U".");
	autoVEC result = from_to_by_VEC (from->number, to->number, by->number);
	pushNumericVector (result.move());
}
static void do_from_to_count_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function “from_to_count#” requires three arguments.");
	const Stackel count = pop, to = pop, from = pop;
	Melder_require (from->which == Stackel_NUMBER,
		U"In the function “from_to_count#”, the first argument should be a number, not ", from->whichText(), U".");
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “from_to_count#”, the second argument should be a number, not ", to->whichText(), U".");
	Melder_require (count->which == Stackel_NUMBER,
		U"In the function “from_to_count#”, the third argument should be a number (the count), not ", count->whichText(), U".");
	autoVEC result = from_to_count_VEC (from->number, to->number, Melder_iround (count->number));
	pushNumericVector (result.move());
}
static void do_between_by_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function “between_by#” requires three arguments.");
	const Stackel by = pop, to = pop, from = pop;
	Melder_require (from->which == Stackel_NUMBER,
		U"In the function “between_by#”, the first argument should be a number, not ", from->whichText(), U".");
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “between_by#”, the second argument should be a number, not ", to->whichText(), U".");
	Melder_require (by->which == Stackel_NUMBER,
		U"In the function “between_by#”, the third argument should be a number (the step), not ", by->whichText(), U".");
	autoVEC result = between_by_VEC (from->number, to->number, by->number);
	pushNumericVector (result.move());
}
static void do_between_count_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 3,
		U"The function “between_count#” requires three arguments.");
	const Stackel count = pop, to = pop, from = pop;
	Melder_require (from->which == Stackel_NUMBER,
		U"In the function “between_count#”, the first argument should be a number, not ", from->whichText(), U".");
	Melder_require (to->which == Stackel_NUMBER,
		U"In the function “between_count#”, the second argument should be a number, not ", to->whichText(), U".");
	Melder_require (count->which == Stackel_NUMBER,
		U"In the function “between_count#”, the third argument should be a number (the count), not ", count->whichText(), U".");
	autoVEC result = between_count_VEC (from->number, to->number, Melder_iround (count->number));
	pushNumericVector (result.move());
}
static void do_sort_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “sort#” requires one argument, namely a vector.");
	const Stackel vec = pop;
	Melder_require (vec->which == Stackel_NUMERIC_VECTOR,
		U"The argument of the function “sort#” should be a numeric vector, not ", vec->whichText(), U".");
	autoVEC result = sort_e_VEC (vec->numericVector);
	pushNumericVector (result.move());
}
static void do_sort_removeUndefined_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “sort_removeUndefined#” requires one argument, namely a vector.");
	const Stackel vec = pop;
	Melder_require (vec->which == Stackel_NUMERIC_VECTOR,
		U"The argument of the function “sort_removeUndefined#” should be a numeric vector, not ", vec->whichText(), U".");
	autoVEC result = sort_removeUndefined_VEC (vec->numericVector);
	pushNumericVector (result.move());
}
static void do_sort_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “sort$#” requires one argument, namely a string array.");
	const Stackel strvec = pop;
	Melder_require (strvec->which == Stackel_STRING_ARRAY,
		U"The argument of the function “sort$#” should be a string array, not ", strvec->whichText(), U".");
	autoSTRVEC result = sort_STRVEC (strvec->stringArray);
	pushStringVector (result.move());
}
static void do_sort_numberAware_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “sort_numberAware$#” requires one argument, namely a string array.");
	const Stackel strvec = pop;
	Melder_require (strvec->which == Stackel_STRING_ARRAY,
		U"The argument of the function “sort_numberAware$#” should be a string array, not ", strvec->whichText(), U".");
	autoSTRVEC result = sort_numberAware_STRVEC (strvec->stringArray);
	pushStringVector (result.move());
}
static void do_shuffle_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “shuffle#” requires one argument, namely a vector.");
	const Stackel vec = pop;
	Melder_require (vec->which == Stackel_NUMERIC_VECTOR,
		U"The argument of the function “shuffle#” should be a numeric vector, not ", vec->whichText(), U".");
	autoVEC result = shuffle_VEC (vec->numericVector);
	pushNumericVector (result.move());
}
static void do_shuffle_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “shuffle$#” requires one argument, namely a string array.");
	const Stackel strvec = pop;
	Melder_require (strvec->which == Stackel_STRING_ARRAY,
		U"The argument of the function “shuffle$#” should be a string array, not ", strvec->whichText(), U".");
	autoSTRVEC result = shuffle_STRVEC (strvec->stringArray);
	pushStringVector (result.move());
}
static void do_peaks_MAT () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 4,
		U"The function peaks## requires four arguments (vector, edges, interpolation, sortByHeight).");
	const Stackel s = pop;
	if (s->which != Stackel_NUMBER)
		Melder_throw (U"The fourth argument to peaks## should be a number, not ", s->whichText(), U".");
	bool sortByHeight = s->number != 0.0;
	const Stackel i = pop;
	if (i->which != Stackel_NUMBER)
		Melder_throw (U"The third argument to peaks## should be a number, not ", i->whichText(), U".");
	integer interpolation = Melder_iround (i->number);
	const Stackel e = pop;
	if (e->which != Stackel_NUMBER)
		Melder_throw (U"The second argument to peaks## should be a number, not ", e->whichText(), U".");
	bool includeEdges = e->number != 0.0;
	const Stackel vec = pop;
	if (vec->which != Stackel_NUMERIC_VECTOR)
		Melder_throw (U"The first argument to peaks## should be a numeric vector, not ", vec->whichText(), U".");
	autoMAT result = peaks_MAT (vec->numericVector, includeEdges, interpolation, sortByHeight);
	pushNumericMatrix (result.move());
}
static void do_size () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “size” requires one (vector) argument.");
	const Stackel array = pop;
	if (array->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (array->numericVector.size);
	} else if (array->which == Stackel_STRING_ARRAY) {
		pushNumber (array->stringArray.size);
	} else {
		Melder_throw (U"The function size requires a vector argument, not ", array->whichText(), U".");
	}
}
static void do_numberOfRows () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “numberOfRows” requires one argument, namely a matrix.");
	const Stackel array = pop;
	if (array->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (array->numericMatrix.nrow);
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a matrix argument, not ", array->whichText(), U".");
	}
}
static void do_numberOfColumns () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “numberOfColumns” requires one argument, namely a matrix.");
	const Stackel array = pop;
	if (array->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (array->numericMatrix.ncol);
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a matrix argument, not ", array->whichText(), U".");
	}
}
static void do_combine_VEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer numberOfArguments = Melder_iround (narg->number);
	stackPointer -= numberOfArguments;
	integer elementCounter = 0;
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER) {
			elementCounter += 1;
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			elementCounter += arg->numericVector.size;
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			elementCounter += arg->numericMatrix.nrow * arg->numericMatrix.ncol;
		} else {
			Melder_throw (U"The function “combine#” only takes numbers, vectors and matrices,"
				" not ", arg->whichText(), U".");
		}
	}
	autoVEC result = raw_VEC (elementCounter);
	integer elementIterator = 0;
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		const Stackel arg = & theStack [stackPointer + iarg];
		if (arg->which == Stackel_NUMBER) {
			result [++ elementIterator] = arg->number;
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++)
				result [++ elementIterator] = arg->numericVector [i];
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++)
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++)
					result [++ elementIterator] = arg->numericMatrix [irow] [icol];
		} else {
			Melder_crash (U"do_combine_VEC should never arrive here.");
		}
	}
	pushNumericVector (result.move());
}
static void do_part_VEC () {
	/*
		Check the number of arguments: always 3.
	*/
	const Stackel s_narg = pop;
	Melder_assert (s_narg->which == Stackel_NUMBER);
	const integer narg = s_narg->number;
	Melder_require (narg == 3,
		U"The function “part#” requires exactly three arguments (namely a vector, a starting index, and an end index), not the ", narg, U" given.");
	/*
		Check the types of the arguments: always vector, number, number.
	*/
	const Stackel s_last = pop, s_first = pop, s_vec = pop;
	Melder_require (s_vec->which == Stackel_NUMERIC_VECTOR,
		U"The first argument of the function “part#” should be a numeric vector, not ", s_vec->whichText(), U".");
	Melder_require (s_first->which == Stackel_NUMBER,
		U"The second argument of the function “part#” should be a number (the starting index), not ", s_first->whichText(), U".");
	Melder_require (s_last->which == Stackel_NUMBER,
		U"The third argument of the function “part#” should be a number (the end index), not ", s_last->whichText(), U".");
	/*
		Check the preconditions of the arguments.
	*/
	const constVEC vec = s_vec->numericVector;
	const integer numberOfElements = vec.size;
	const integer first = Melder_iround (s_first->number);
	Melder_require (first > 0,
		U"The second argument of the function “part#” (the starting index) should (after rounding) be a positive whole number, not ", first, U".");
	Melder_require (first <= numberOfElements,
		U"The second argument of the function “part#” (the starting index) should (after rounding) be at most the number of elements (",
		numberOfElements, U"), not ", first, U"."
	);
	const integer last = Melder_iround (s_last->number);
	Melder_require (last > 0,
		U"The third argument of the function “part#” (the end index) should (after rounding) be a positive whole number, not ", last, U".");
	Melder_require (last <= numberOfElements,
		U"The third argument of the function “part#” (the end index) should (after rounding) be at most the number of elements (",
		numberOfElements, U"), not ", last, U"."
	);

	const integer newSize = last - (first - 1);
	if (newSize > 0)
		pushNumericVector (copy_VEC (vec.part (first, last)));
	else
		pushNumericVector (autoVEC ());
}
static void do_part_MAT () {
	/*
		Check the number of arguments: always 5.
	*/
	const Stackel s_narg = pop;
	Melder_assert (s_narg->which == Stackel_NUMBER);
	const integer narg = s_narg->number;
	Melder_require (narg == 5,
		U"The function “part##” requires exactly five arguments (namely a matrix, a starting row, an end row, a starting column, and an end column), not the ", narg, U" given..");
	/*
		Check the types of the arguments: always matrix, number, number, number, number.
	*/
	const Stackel s_endColumn = pop, s_startingColumn = pop, s_endRow = pop, s_startingRow = pop, s_mat = pop;
	Melder_require (s_mat->which == Stackel_NUMERIC_MATRIX,
		U"The first argument of the function “part##” should be a numeric matrix, not ", s_mat->whichText(), U".");
	Melder_require (s_startingRow->which == Stackel_NUMBER,
		U"The second argument of the function “part##” should be a number (the starting row), not ", s_startingRow->whichText(), U".");
	Melder_require (s_endRow->which == Stackel_NUMBER,
		U"The third argument of the function “part##” should be a number (the end row), not ", s_endRow->whichText(), U".");
	Melder_require (s_startingColumn->which == Stackel_NUMBER,
		U"The fourth argument of the function “part##” should be a number (the starting column), not ", s_startingColumn->whichText(), U".");
	Melder_require (s_endColumn->which == Stackel_NUMBER,
		U"The fifth argument of the function “part##” should be a number (the end column), not ", s_endColumn->whichText(), U".");
	/*
		Check the preconditions of the arguments.
	*/
	const constMAT mat = s_mat->numericMatrix;
	const integer numberOfRows = mat.nrow, numberOfColumns = mat.ncol;
	const integer startingRow = Melder_iround (s_startingRow->number);
	Melder_require (startingRow > 0,
		U"The second argument of the function “part##” (the starting row) should (after rounding) be a positive whole number, not ", startingRow, U".");
	Melder_require (startingRow <= numberOfRows,
		U"The second argument of the function “part##” (the starting row) should (after rounding) be at most the number of rows (",
		numberOfRows, U"), not ", startingRow, U"."
	);
	const integer endRow = Melder_iround (s_endRow->number);
	Melder_require (endRow > 0,
		U"The third argument of the function “part##” (the end row) should (after rounding) be a positive whole number, not ", endRow, U".");
	Melder_require (endRow <= numberOfRows,
		U"The third argument of the function “part##” (the end row) should (after rounding) be at most the number of rows (",
		numberOfRows, U"), not ", endRow, U"."
	);
	const integer startingColumn = Melder_iround (s_startingColumn->number);
	Melder_require (startingColumn > 0,
		U"The fourth argument of the function “part##” (the starting column) should (after rounding) be a positive whole number, not ", startingColumn, U".");
	Melder_require (startingColumn <= numberOfColumns,
		U"The fourth argument of the function “part##” (the starting column) should (after rounding) be at most the number of columns (",
		numberOfColumns, U"), not ", startingColumn, U"."
	);
	const integer endColumn = Melder_iround (s_endColumn->number);
	Melder_require (endColumn > 0,
		U"The fifth argument of the function “part##” (the end column) should (after rounding) be a positive whole number, not ", endColumn, U".");
	Melder_require (endColumn <= numberOfColumns,
		U"The fifth argument of the function “part##” (the end column) should (after rounding) be at most the number of columns (",
		numberOfColumns, U"), not ", endColumn, U"."
	);
	pushNumericMatrix (copy_MAT (mat.part (startingRow, endRow, startingColumn, endColumn)));
}
static void do_editor () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 0) {
		/*
			No editor mentioned, so switch back to the already existing editor.
		*/
		if (theInterpreter && theInterpreter -> wasStartedFromEditorEnvironment())
			theInterpreter -> setDynamicFromOwningEditorEnvironment ();
		else
			Melder_throw (U"The function “editor” requires an argument when called from outside an editor.");
	} else if (narg->number == 1) {
		const Stackel editor = pop;
		if (editor->which == Stackel_STRING) {
			theInterpreter -> setDynamicEditorEnvironmentFromEditor (praat_findEditorFromString (editor->getString()));
		} else if (editor->which == Stackel_NUMBER) {
			theInterpreter -> setDynamicEditorEnvironmentFromEditor (praat_findEditorById (Melder_iround (editor->number)));
		} else {
			Melder_throw (U"The function “editor” requires a numeric or string argument, not ", editor->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “editor” requires 0 or 1 arguments, not ", narg->number, U".");
	}
	pushNumber (1);
}
static void do_random_initializeWithSeedUnsafelyButPredictably () {
	/*@praat
		#
		# A published test: the 10,000th element of the default 64-bit Mersenne Twister random sequence
		# should be 9'981'545'732'273'789'042.
		#
		random_initializeWithSeedUnsafelyButPredictably (5489)
		for i to 10000
			a = randomUniform (0, 1)
		endfor
		random_initializeSafelyAndUnpredictably ()
		a *= 2 ^ 64
		roundingError = a - 9981545732273789042
		writeInfoLine: fixed$ (a, 0), " ", roundingError
		assert roundingError <= 2048   ; actually usually zero
	@*/
	const Stackel narg = pop;
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_NUMBER) {
			const double seed = s->number;
			if (seed < 0.0 || seed > double (INT54_MAX))
				Melder_throw (U"For the function “random_initializeWithSeedUnsafelyButPredictably”, "
						"the seed should be between 0 and ", double (INT54_MAX), U".");
			NUMrandom_initializeWithSeedUnsafelyButPredictably (uint64 (round (seed)));
		} else {
			Melder_throw (U"The function “hash” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “random_initializeWithSeedUnsafelyButPredictably” requires 1 argument (the seed), not ", narg->number, U".");
	}
	pushNumber (1);
}
static void do_random_initializeSafelyAndUnpredictably () {
	const Stackel narg = pop;
	Melder_require (narg->number == 0,
		U"The function “random_initializeSafelyAndUnpredictably” requires 0 arguments, not ", narg->number, U".");
	NUMrandom_initializeSafelyAndUnpredictably ();
	pushNumber (1);
}
static void do_hash () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			const double result = NUMhashString (s->getString());
			pushNumber (result);
		} else {
			Melder_throw (U"The function “hash” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “hash” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_hex_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = hex_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “hex$” requires a string, not ", s->whichText(), U".");
		}
	} else if (narg->number == 2) {
		const Stackel k = pop, s = pop;
		if (s->which == Stackel_STRING && k->which == Stackel_NUMBER) {
			autostring32 result = hex_STR (s->getString(), uint64 (round (k->number)));
			pushString (result.move());
		} else {
			Melder_throw (U"The function “hex$” requires a string and a number, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “hex$” requires 1 or 2 arguments, not ", narg->number, U".");
	}
}
static void do_unhex_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = unhex_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “unhex$” requires a string, not ", s->whichText(), U".");
		}
	} else if (narg->number == 2) {
		const Stackel k = pop, s = pop;
		if (s->which == Stackel_STRING && k->which == Stackel_NUMBER) {
			autostring32 result = unhex_STR (s->getString(), uint64 (round (k->number)));
			pushString (result.move());
		} else {
			Melder_throw (U"The function “unhex$” requires a string and a number, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “unhex$” requires 1 or 2 arguments, not ", narg->number, U".");
	}
}
static void do_empty_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “empty$#” requires one argument, namely the number of elements.");
	const Stackel numberOfElements = pop;
	Melder_require (numberOfElements->which == Stackel_NUMBER,
		U"The argument of the function “empty$#” should be a number (namely the number of elements), not ", numberOfElements->whichText(), U".");
	autoSTRVEC result { Melder_iround (numberOfElements->number) };
	pushStringVector (result.move());
}
static void do_readLinesFromFile_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “readLinesFromFile$#” requires one argument, namely the file pattern.");
	const Stackel fileName = pop;
	if (fileName->which != Stackel_STRING)
		Melder_throw (U"The argument of the function “readLinesFromFile$#” should be a string (namely the file pattern), not ", fileName->whichText(), U".");
	structMelderFile file { };
	Melder_relativePathToFile (fileName->getString(), & file);
	autoSTRVEC result = readLinesFromFile_STRVEC (& file);
	pushStringVector (result.move());
}
static void do_fileNames_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “fileNames$#” requires one argument, namely the file pattern.");
	const Stackel filePattern = pop;
	if (filePattern->which != Stackel_STRING)
		Melder_throw (U"The argument of the function “fileNames$#” should be a string (namely the file path and pattern), not ", filePattern->whichText(), U".");
	autoSTRVEC result = fileNames_STRVEC (filePattern->getString());
	pushStringVector (result.move());
}
static void do_folderNames_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “folderNames$#” requires one argument, namely the file pattern.");
	const Stackel folderPattern = pop;
	Melder_require (folderPattern->which == Stackel_STRING,
		U"The argument of the function “folderNames$#” should be a string (namely the folder path), not ", folderPattern->whichText(), U".");
	autoSTRVEC result = folderNames_STRVEC (folderPattern->getString());
	pushStringVector (result.move());
}
static void do_fileNames_caseInsensitive_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “fileNames_caseInsensitive$#” requires one argument, namely the file pattern.");
	const Stackel filePattern = pop;
	if (filePattern->which != Stackel_STRING)
		Melder_throw (U"The argument of the function “fileNames_caseInsensitive$#” should be a string (namely the file path and pattern), not ", filePattern->whichText(), U".");
	autoSTRVEC result = fileNames_caseInsensitive_STRVEC (filePattern->getString());
	pushStringVector (result.move());
}
static void do_folderNames_caseInsensitive_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “folderNames_caseInsensitive$#” requires one argument, namely the file pattern.");
	const Stackel folderPattern = pop;
	Melder_require (folderPattern->which == Stackel_STRING,
		U"The argument of the function “folderNames_caseInsensitive$#” should be a string (namely the folder path), not ", folderPattern->whichText(), U".");
	autoSTRVEC result = folderNames_caseInsensitive_STRVEC (folderPattern->getString());
	pushStringVector (result.move());
}
static void do_splitByWhitespace_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg -> which == Stackel_NUMBER);
	Melder_require (narg->number == 1,
		U"The function “splitByWhitespace$#” requires one argument, namely the string to split.");
	const Stackel string = pop;
	Melder_require (string->which == Stackel_STRING,
		U"The argument of the function “splitByWhitespace$#” should be a string, not ", string->whichText(), U".");
	autoSTRVEC result = splitByWhitespace_STRVEC (string->getString());
	pushStringVector (result.move());
}
static void do_splitBy_STRVEC () {
	const Stackel narg = pop;
	Melder_assert (narg -> which == Stackel_NUMBER);
	Melder_require (narg->number == 2,
		U"The function “splitBy$#” requires two arguments, namely the string to split and the separator.");
	const Stackel separator = pop, string = pop;
	Melder_require (string->which == Stackel_STRING,
		U"The first argument of the function “splitBy$#” should be a string, not ", string->whichText(), U".");
	Melder_require (separator->which == Stackel_STRING,
		U"The second argument of the function “splitBy$#” (the separator) should be a string, not ", string->whichText(), U".");
	autoSTRVEC result = splitBy_STRVEC (string->getString(), separator->getString());
	pushStringVector (result.move());
}
static void do_lowerCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = lowerCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “lowerCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “lowerCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_lowerCamelCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = lowerCamelCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “lowerCamelCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “lowerCamelCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_lowerSnakeCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = lowerSnakeCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “lowerSnakeCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “lowerSnakeCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_upperCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = upperCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “upperCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “upperCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_upperCamelCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = upperCamelCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “upperCamelCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “upperCamelCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_upperSnakeCase_STR () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			autostring32 result = upperSnakeCase_STR (s->getString());
			pushString (result.move());
		} else {
			Melder_throw (U"The function “upperSnakeCase$” requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “upperSnakeCase$” requires 1 argument, not ", narg->number, U".");
	}
}
static void do_numericVectorElement () {
	InterpreterVariable vector = parse [programPointer]. content.variable;
	const Stackel element = pop;
	Melder_require (element->which == Stackel_NUMBER,
		U"In vector indexing, the index should be a number, not ", element->whichText(), U".");
	Melder_require (isdefined (element->number),
		U"The element index is undefined.");
	const integer ielement = Melder_iround (element->number);
	Melder_require (ielement > 0,
		U"In vector indexing, the element index should be positive.");
	Melder_require (ielement <= vector->numericVectorValue.size,
		U"Element index out of bounds. Looking for element ", ielement,
		U", but there are only ", vector->numericVectorValue.size, U" elements."
	);
	pushNumber (vector->numericVectorValue [ielement]);
}
static void do_numericMatrixElement () {
	InterpreterVariable matrix = parse [programPointer]. content.variable;
	const Stackel column = pop;
	Melder_require (column->which == Stackel_NUMBER,
		U"In matrix indexing, the column index should be a number, not ", column->whichText(), U".");
	Melder_require (isdefined (column->number),
		U"The column index is undefined.");
	const integer icolumn = Melder_iround (column->number);
	Melder_require (icolumn > 0,
		U"In matrix indexing, the column index should be positive.");
	Melder_require (icolumn <= matrix->numericMatrixValue. ncol,
		U"Column index out of bounds. Looking for column ", icolumn,
		U", but there are only ", matrix->numericMatrixValue. ncol, U" columns."
	);
	const Stackel row = pop;
	Melder_require (row->which == Stackel_NUMBER,
		U"In matrix indexing, the row index should be a number, not ", row->whichText(), U".");
	Melder_require (isdefined (row->number),
		U"The row index is undefined.");
	const integer irow = Melder_iround (row->number);
	Melder_require (irow > 0,
		U"In matrix indexing, the row index should be positive.");
	Melder_require (irow <= matrix->numericMatrixValue. nrow,
		U"Row index out of bounds. Looking for row ", irow,
		U", but there are only ", matrix->numericMatrixValue. nrow, U" rows."
	);
	pushNumber (matrix->numericMatrixValue [irow] [icolumn]);
}
static void do_stringVectorElement () {
	InterpreterVariable vector = parse [programPointer]. content.variable;
	const Stackel element = pop;
	Melder_require (element->which == Stackel_NUMBER,
		U"In vector indexing, the index should be a number, not ", element->whichText(), U".");
	Melder_require (isdefined (element->number),
		U"The element index is undefined.");
	const integer ielement = Melder_iround (element->number);
	Melder_require (ielement > 0,
		U"In vector indexing, the element index should be positive.");
	Melder_require (ielement <= vector->stringArrayValue.size,
		U"Element index out of bounds. Looking for element ", ielement,
		U", but there are only ", vector->stringArrayValue.size, U" elements."
	);
	pushString (Melder_dup (vector->stringArrayValue [ielement].get()));
}
static void do_indexedNumericVariable () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer nindex = Melder_iround (narg->number);
	Melder_require (nindex >= 1,
		U"Indexed variables require at least one index.");
	char32 *indexedVariableName = parse [programPointer]. content.string;
	static MelderString totalVariableName;
	MelderString_copy (& totalVariableName, indexedVariableName, U"[");
	stackPointer -= nindex;
	for (int iindex = 1; iindex <= nindex; iindex ++) {
		const Stackel index = & theStack [stackPointer + iindex];
		if (index->which == Stackel_NUMBER) {
			MelderString_append (& totalVariableName, index->number, iindex == nindex ? U"]" : U",");
		} else if (index -> which == Stackel_STRING) {
			MelderString_append (& totalVariableName, U"\"", index->getString(), U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index should be a number or a string, not ", index->whichText(), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	Melder_require (!! var,
		U"Undefined indexed variable «", totalVariableName.string, U"».");
	pushNumber (var -> numericValue);
}
static void do_indexedStringVariable () {
	const Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	const integer nindex = Melder_iround (narg->number);
	Melder_require (nindex >= 1,
		U"Indexed variables require at least one index.");
	char32 *indexedVariableName = parse [programPointer]. content.string;
	static MelderString totalVariableName;
	MelderString_copy (& totalVariableName, indexedVariableName, U"[");
	stackPointer -= nindex;
	for (int iindex = 1; iindex <= nindex; iindex ++) {
		const Stackel index = & theStack [stackPointer + iindex];
		if (index->which == Stackel_NUMBER) {
			MelderString_append (& totalVariableName, index -> number, iindex == nindex ? U"]" : U",");
		} else if (index -> which == Stackel_STRING) {
			MelderString_append (& totalVariableName, U"\"", index -> getString(), U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index should be a number or a string, not ", index->whichText(), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	Melder_require (!! var,
		U"Undefined indexed variable «", totalVariableName.string, U"».");
	autostring32 result = Melder_dup (var -> stringValue.get());
	pushString (result.move());
}
static void do_length () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		const double result = NUMlength (s->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function “length” requires a string, not ", s->whichText(), U".");
	}
}
static void do_number () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = Melder_atof (s->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function “number” requires a string, not ", s->whichText(), U".");
	}
}
static void do_fileReadable () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (s->getString(), & file);
		pushNumber (MelderFile_readable (& file));
	} else {
		Melder_throw (U"The function “fileReadable” requires a string, not ", s->whichText(), U".");
	}
}
static void do_folderExists () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFolder folder { };
		Melder_relativePathToFolder (s->getString(), & folder);
		pushNumber (MelderFolder_exists (& folder));
	} else {
		Melder_throw (U"The function “folderExists” requires a string, not ", s->whichText(), U".");
	}
}
static void do_tryToWriteFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “tryToWriteFile” is not available inside manuals.");
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (s->getString(), & file);
		pushNumber (Melder_tryToWriteFile (& file));
	} else {
		Melder_throw (U"The function “tryToWriteFile” requires a string, not ", s->whichText(), U".");
	}
}
static void do_tryToAppendFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “tryToAppendFile” is not available inside manuals.");
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (s->getString(), & file);
		pushNumber (Melder_tryToAppendFile (& file));
	} else {
		Melder_throw (U"The function “tryToAppendFile” requires a string, not ", s->whichText(), U".");
	}
}
static void do_date_STR () {
	pushString (date_STR ());
}
static void do_date_utc_STR () {
	pushString (date_utc_STR ());
}
static void do_date_iso_STR () {
	pushString (date_iso_STR ());
}
static void do_date_utc_iso_STR () {
	pushString (date_utc_iso_STR ());
}
static void do_date_VEC () {
	pushNumericVector (date_VEC ());
}
static void do_date_utc_VEC () {
	pushNumericVector (date_utc_VEC ());
}
static void do_info_STR () {
	autostring32 info = Melder_dup (Melder_getInfo ());
	pushString (info.move());
}
static void do_left_STR () {
	const Stackel narg = pop;
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			pushString (left_STR (s->getString()));
		} else {
			Melder_throw (U"The function “left$” requires a string (or a string and a number).");
		}
	} else if (narg->number == 2) {
		const Stackel n = pop, s = pop;
		if (s->which == Stackel_STRING && n->which == Stackel_NUMBER) {
			pushString (left_STR (s->getString(), Melder_iround (n->number)));
		} else {
			Melder_throw (U"The function “left$” requires a string and a number (or a string only).");
		}
	} else {
		Melder_throw (U"The function “left$” requires one or two arguments: a string and optionally a number.");
	}
	trace (U"exit");
}
static void do_right_STR () {
	const Stackel narg = pop;
	if (narg->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			pushString (right_STR (s->getString()));
		} else {
			Melder_throw (U"The function “right$” requires a string (or a string and a number).");
		}
	} else if (narg->number == 2) {
		const Stackel n = pop, s = pop;
		if (s->which == Stackel_STRING && n->which == Stackel_NUMBER) {
			pushString (right_STR (s->getString(), Melder_iround (n->number)));
		} else {
			Melder_throw (U"The function “right$” requires a string and a number (or a string only).");
		}
	} else {
		Melder_throw (U"The function “right$” requires one or two arguments: a string and optionally a number.");
	}
}
static void do_mid_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel position = pop, str = pop;
		if (str->which == Stackel_STRING && position->which == Stackel_NUMBER) {
			pushString (mid_STR (str->getString(), Melder_iround (position->number)));
		} else {
			Melder_throw (U"The function “mid$” requires a string and a number (or two).");
		}
	} else if (narg->number == 3) {
		const Stackel numberOfCharacters = pop, startingPosition = pop, str = pop;
		if (str->which == Stackel_STRING && startingPosition->which == Stackel_NUMBER && numberOfCharacters->which == Stackel_NUMBER) {
			pushString (mid_STR (str->getString(), Melder_iround (startingPosition->number), Melder_iround (numberOfCharacters->number)));
		} else {
			Melder_throw (U"The function “mid$” requires a string and two numbers (or one).");
		}
	} else {
		Melder_throw (U"The function “mid$” requires two or three arguments.");
	}
}
static void do_padLeft_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (padLeft_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “padLeft$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else if (narg->number == 3) {
		const Stackel pad = pop, width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER && pad->which == Stackel_STRING)
			pushString (padLeft_STR (str->getString(), Melder_iround (width->number), pad->getString()));
		else
			Melder_throw (U"The three arguments to “padLeft$” should be a string, a number (the desired width),"
				U" and a string (the pad), not ", str->whichText(), U", ", width->whichText(),
				U" and ", pad->whichText(), U"."
			);
	} else
		Melder_throw (U"The function “padLeft$” requires two or three arguments (a string, a number, and an optional pad string).");
}
static void do_padRight_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (padRight_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “padRight$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else if (narg->number == 3) {
		const Stackel pad = pop, width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER && pad->which == Stackel_STRING)
			pushString (padRight_STR (str->getString(), Melder_iround (width->number), pad->getString()));
		else
			Melder_throw (U"The three arguments to “padRight$” should be a string, a number (the desired width),"
				U" and a string (the pad), not ", str->whichText(), U", ", width->whichText(),
				U" and ", pad->whichText(), U"."
			);
	} else
		Melder_throw (U"The function “padRight$” requires two or three arguments (a string, a number, and an optional pad string).");
}
static void do_truncateLeft_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (truncateLeft_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “truncateLeft$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else
		Melder_throw (U"The function “truncateLeft$” requires two arguments: a string and a number (the desired width).");
}
static void do_truncateRight_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (truncateRight_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “truncateRight$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else
		Melder_throw (U"The function “truncateRight$” requires two arguments: a string and a number (the desired width).");
}
static void do_padOrTruncateLeft_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (padOrTruncateLeft_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “padOrTruncateLeft$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else if (narg->number == 3) {
		const Stackel pad = pop, width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER && pad->which == Stackel_STRING)
			pushString (padOrTruncateLeft_STR (str->getString(), Melder_iround (width->number), pad->getString()));
		else
			Melder_throw (U"The three arguments to “padOrTruncateLeft$” should be a string, a number (the desired width),"
				U" and a string (the pad), not ", str->whichText(), U", ", width->whichText(),
				U" and ", pad->whichText(), U"."
			);
	} else
		Melder_throw (U"The function “padOrTruncateLeft$” requires two or three arguments (a string, a number, and an optional pad string).");
}
static void do_padOrTruncateRight_STR () {
	const Stackel narg = pop;
	if (narg->number == 2) {
		const Stackel width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER)
			pushString (padOrTruncateRight_STR (str->getString(), Melder_iround (width->number)));
		else
			Melder_throw (U"The two arguments to “padOrTruncateRight$” should be a string and a number (the desired width), not ",
					str->whichText(), U" and ", width->whichText(), U".");
	} else if (narg->number == 3) {
		const Stackel pad = pop, width = pop, str = pop;
		if (str->which == Stackel_STRING && width->which == Stackel_NUMBER && pad->which == Stackel_STRING)
			pushString (padOrTruncateRight_STR (str->getString(), Melder_iround (width->number), pad->getString()));
		else
			Melder_throw (U"The three arguments to “padOrTruncateRight$” should be a string, a number (the desired width),"
				U" and a string (the pad), not ", str->whichText(), U", ", width->whichText(),
				U" and ", pad->whichText(), U"."
			);
	} else
		Melder_throw (U"The function “padOrTruncateRight$” requires two or three arguments (a string, a number, and an optional pad string).");
}
static void do_unicodeToBackslashTrigraphs_STR () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		const integer length = Melder_length (s->getString());
		autostring32 trigraphs (3 * length);
		Longchar_genericize (s->getString(), trigraphs.get());
		pushString (trigraphs.move());
	} else {
		Melder_throw (U"The function “unicodeToBackslashTrigraphs$” requires a string, not ", s->whichText(), U".");
	}
}
static void do_backslashTrigraphsToUnicode_STR () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		const integer length = Melder_length (s->getString());
		autostring32 unicode (length);
		Longchar_nativize (s->getString(), unicode.get(), false);   // noexcept
		pushString (unicode.move());
	} else {
		Melder_throw (U"The function “unicodeToBackslashTrigraphs$” requires a string, not ", s->whichText(), U".");
	}
}
static void do_environment_STR () {
	const Stackel s = pop;
	if (s->which == Stackel_STRING) {
		const conststring32 value = Melder_getenv (s->getString());
		autostring32 result = Melder_dup (value ? value : U"");
		pushString (result.move());
	} else {
		Melder_throw (U"The function “environment$” requires a string, not ", s->whichText(), U".");
	}
}
static void do_index () {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		const integer result = ( substring ? substring - s->getString() + 1 : 0 );   // 0 is the special case, meaning "not found"
		pushNumber (result);
	} else if (s->which == Stackel_STRING_ARRAY && t->which == Stackel_STRING) {
		pushNumber (NUMfindFirst (s->stringArray, t->getString()));
	} else {
		Melder_throw (U"The function “index” requires two strings, not ",
			s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_index_caseInsensitive () {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str_caseInsensitive (s->getString(), t->getString());
		const integer result = ( substring ? substring - s->getString() + 1 : 0 );   // 0 is the special case, meaning "not found"
		pushNumber (result);
	} else if (s->which == Stackel_STRING_ARRAY && t->which == Stackel_STRING) {
		pushNumber (NUMfindFirst_caseInsensitive (s->stringArray, t->getString()));
	} else {
		Melder_throw (U"The function “index_caseInsensitive” requires two strings, not ",
			s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_rindex () {
	const Stackel part = pop, whole = pop;
	if (whole->which == Stackel_STRING && part->which == Stackel_STRING) {
		char32 *lastSubstring = str32str (whole->getString(), part->getString());
		if (part->getString() [0] == U'\0') {
			const integer result = Melder_length (whole->getString());
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				char32 *substring = str32str (lastSubstring + 1, part->getString());
				if (! substring)
					break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole->getString() + 1);
		} else {
			pushNumber (0);   // 0 is the special case, meaning "not found"
		}
	} else if (whole->which == Stackel_STRING_ARRAY && part->which == Stackel_STRING) {
		pushNumber (NUMfindLast (whole->stringArray, part->getString()));
	} else {
		Melder_throw (U"The function “rindex” requires two strings, not ",
			whole->whichText(), U" and ", part->whichText(), U".");
	}
}
static void do_rindex_caseInsensitive () {
	const Stackel part = pop, whole = pop;
	if (whole->which == Stackel_STRING && part->which == Stackel_STRING) {
		char32 *lastSubstring = str32str_caseInsensitive (whole->getString(), part->getString());
		if (part->getString() [0] == U'\0') {
			const integer result = Melder_length (whole->getString());
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				char32 *substring = str32str_caseInsensitive (lastSubstring + 1, part->getString());
				if (! substring)
					break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole->getString() + 1);
		} else {
			pushNumber (0);   // 0 is the special case, meaning "not found"
		}
	} else if (whole->which == Stackel_STRING_ARRAY && part->which == Stackel_STRING) {
		pushNumber (NUMfindLast_caseInsensitive (whole->stringArray, part->getString()));
	} else {
		Melder_throw (U"The function “rindex_caseInsensitive” requires two strings, not ",
			whole->whichText(), U" and ", part->whichText(), U".");
	}
}
static void do_index_regex (int backward) {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		conststring32 errorMessage;
		regexp *compiled_regexp = CompileRE (t->getString(), & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"index_regex(): ", errorMessage, U".");
		} else {
			if (ExecRE (compiled_regexp, nullptr, s->getString(), nullptr, backward, U'\0', U'\0', nullptr, nullptr)) {
				char32 *location = (char32 *) compiled_regexp -> startp [0];
				pushNumber (location - s->getString() + 1);
				free (compiled_regexp);
			} else {
				pushNumber (false);
			}
		}
	} else {
		Melder_throw (U"The function “", Formula_instructionNames [parse [programPointer]. symbol],
			U"” requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_stringMatchesCriterion (kMelder_string criterion, bool caseSensitive) {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		const bool result = Melder_stringMatchesCriterion (s->getString(), criterion, t->getString(), caseSensitive);
		pushNumber (result);
	} else {
		Melder_throw (U"The function “", Formula_instructionNames [parse [programPointer]. symbol],
			U"” requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_extractNumber () {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		if (! substring) {
			pushNumber (undefined);
		} else {
			/* Skip the prompt. */
			substring += Melder_length (t->getString());
			/* Skip white space. */
			while (Melder_isHorizontalOrVerticalSpace (*substring)) substring ++;
			if (substring [0] == U'\0' || str32nequ (substring, U"--undefined--", 13)) {
				pushNumber (undefined);
			} else {
				char32 buffer [101];
				int i = 0;
				for (; i < 100; i ++) {
					buffer [i] = *substring;
					substring ++;
					if (*substring == U'\0' || Melder_isHorizontalOrVerticalSpace (*substring))
						break;
				}
				if (i >= 100) {
					buffer [100] = U'\0';
					pushNumber (Melder_atof (buffer));
				} else {
					buffer [i + 1] = U'\0';
					char32 *slash = str32chr (buffer, U'/');
					if (slash) {
						*slash = U'\0';
						double numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
						pushNumber (numerator / denominator);
					} else {
						pushNumber (Melder_atof (buffer));
					}
				}
			}
		}
	} else {
		Melder_throw (U"The function “", Formula_instructionNames [parse [programPointer]. symbol],
			U"” requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_extractText_STR (bool singleWord) {
	const Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		autostring32 result;
		if (! substring) {
			result = Melder_dup (U"");
		} else {
			integer length;
			/* Skip the prompt. */
			substring += Melder_length (t->getString());
			if (singleWord) {
				/* Skip white space. */
				while (Melder_isHorizontalOrVerticalSpace (*substring)) substring ++;
			}
			char32 *p = substring;
			if (singleWord) {
				/* Proceed until next white space. */
				while (Melder_staysWithinInk (*p)) p ++;
			} else {
				/* Proceed until end of line. */
				while (Melder_staysWithinLine (*p)) p ++;
			}
			length = p - substring;
			result = autostring32 (length);
			str32ncpy (result.get(), substring, length);
		}
		pushString (result.move());
	} else {
		Melder_throw (U"The function “", Formula_instructionNames [parse [programPointer]. symbol],
			U"” requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_replace_STR () {
	const Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		autostring32 result = replace_STR (s->getString(), t->getString(), u->getString(), Melder_iround (x->number));
		pushString (result.move());
	} else {
		Melder_throw (U"The function “replace$” requires three strings and a number.");
	}
}
static void do_replace_regex_STR () {
	const Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		conststring32 errorMessage;
		regexp *compiled_regexp = CompileRE (t->getString(), & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"replace_regex$(): ", errorMessage, U".");
		} else {
			autostring32 result = replace_regex_STR (s->getString(), compiled_regexp, u->getString(), Melder_iround (x->number));
			pushString (result.move());
		}
	} else {
		Melder_throw (U"The function “replace_regex$” requires three strings and a number.");
	}
}
static void do_selected () {
	const Stackel n = pop;
	integer result = 0;
	if (n->number == 0) {
		result = praat_idOfSelected (nullptr, 0);
	} else if (n->number == 1) {
		const Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->getString(), nullptr);
			result = praat_idOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			result = praat_idOfSelected (nullptr, Melder_iround (a->number));
		} else {
			Melder_throw (U"The function “selected” requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		const Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			const ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_idOfSelected (klas, Melder_iround (x->number));
		} else {
			Melder_throw (U"The function “selected” requires a string (an object type name) and/or a number.");
		}
	} else {
		Melder_throw (U"The function “selected” requires 0, 1, or 2 arguments, not ", n->number, U".");
	}
	pushNumber (result);
}
static void do_selected_STR () {
	const Stackel n = pop;
	char32 *resultSource;   // purposefully don't initialize, so that the compiler can check that has been assigned to when used
	if (n->number == 0) {
		resultSource = praat_nameOfSelected (nullptr, 0);
	} else if (n->number == 1) {
		const Stackel a = pop;
		if (a->which == Stackel_STRING) {
			const ClassInfo klas = Thing_classFromClassName (a->getString(), nullptr);
			resultSource = praat_nameOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			resultSource = praat_nameOfSelected (nullptr, Melder_iround (a->number));
		} else {
			Melder_throw (U"The function “selected$” requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		const Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			resultSource = praat_nameOfSelected (klas, Melder_iround (x->number));
		} else {
			Melder_throw (U"The function “selected$” requires a string (an object type name) and a number.");
		}
	} else {
		Melder_throw (U"The function “selected$” requires 0, 1, or 2 arguments, not ", n->number, U".");
	}
	pushString (Melder_dup (resultSource));
}
static void do_numberOfSelected () {
	const Stackel n = pop;
	integer result = 0;
	if (n->number == 0) {
		result = praat_numberOfSelected (nullptr);
	} else if (n->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			const ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_numberOfSelected (klas);
		} else {
			Melder_throw (U"The function “numberOfSelected” requires a string (an object type name), not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “numberOfSelected” requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumber (result);
}
static void do_selected_VEC () {
	const Stackel n = pop;
	autoVEC result;
	if (n->number == 0) {
		result = praat_idsOfAllSelected (nullptr);
	} else if (n->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			const ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_idsOfAllSelected (klas);
		} else {
			Melder_throw (U"The function “selected#” requires a string (an object type name), not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “selected#” requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumericVector (result.move());
}
static void do_selected_STRVEC () {
	const Stackel n = pop;
	autoSTRVEC result;
	if (n->number == 0) {
		result = praat_namesOfAllSelected (nullptr);
	} else if (n->number == 1) {
		const Stackel s = pop;
		if (s->which == Stackel_STRING) {
			const ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_namesOfAllSelected (klas);
		} else {
			Melder_throw (U"The function “selected$#” requires a string (an object type name), not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “selected$#” requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushStringVector (result.move());
}
static void do_selectObject () {
	const Stackel n = pop;
	praat_deselectAll ();
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		const Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			const integer IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			const integer IOBJECT = praat_findObjectByName (object -> getString());
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			const VEC vec = object -> numericVector;
			for (integer ielm = 1; ielm <= vec.size; ielm ++) {
				const integer IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_select (IOBJECT);
			}
		} else {
			Melder_throw (U"The function “selectObject” takes numbers, strings, or numeric vectors, not ", object->whichText());
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_plusObject () {
	const Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		const Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			const integer IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			const integer IOBJECT = praat_findObjectByName (object -> getString());
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			const VEC vec = object -> numericVector;
			for (integer ielm = 1; ielm <= vec.size; ielm ++) {
				const integer IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_select (IOBJECT);
			}
		} else {
			Melder_throw (U"The function “plusObject” takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_minusObject () {
	const Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		const Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			const integer IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_deselect (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			const integer IOBJECT = praat_findObjectByName (object -> getString());
			praat_deselect (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			const VEC vec = object -> numericVector;
			for (integer ielm = 1; ielm <= vec.size; ielm ++) {
				const integer IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_deselect (IOBJECT);
			}
		} else {
			Melder_throw (U"The function “minusObject” takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_removeObject () {
	const Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		const Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			const integer IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_removeObject (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			const integer IOBJECT = praat_findObjectByName (object -> getString());
			praat_removeObject (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			const VEC vec = object -> numericVector;
			for (integer ielm = 1; ielm <= vec.size; ielm ++) {
				const integer IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_removeObject (IOBJECT);
			}
		} else {
			Melder_throw (U"The function “removeObject” takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static Daata _do_object (Stackel object, conststring32 expressionMessage) {
	Daata data;
	if (object -> which == Stackel_NUMBER) {
		const integer IOBJECT = praat_findObjectById (Melder_iround (object -> number));
		data = OBJECT;
	} else if (object -> which == Stackel_STRING) {
		const integer IOBJECT = praat_findObjectByName (object -> getString());
		data = OBJECT;
	} else if (object -> which == Stackel_OBJECT) {
		data = object -> object;
	} else {
		Melder_throw (U"The expression “", expressionMessage, U"” requires xx to be a number or a string, not ", object->whichText(), U".");
	}
	return data;
}
static void do_object_xmin () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].xmin");
	Melder_require (data -> v_hasGetXmin (),
		U"An object of type ", Thing_className (data), U" has no “xmin” attribute.");
	pushNumber (data -> v_getXmin ());
}
static void do_object_xmax () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].xmax");
	Melder_require (data -> v_hasGetXmax (),
		U"An object of type ", Thing_className (data), U" has no “xmax” attribute.");
	pushNumber (data -> v_getXmax ());
}
static void do_object_ymin () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].ymin");
	Melder_require (data -> v_hasGetYmin (),
		U"An object of type ", Thing_className (data), U" has no “ymin” attribute.");
	pushNumber (data -> v_getYmin ());
}
static void do_object_ymax () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].ymax");
	Melder_require (data -> v_hasGetYmax (),
		U"An object of type ", Thing_className (data), U" has no “ymax” attribute.");
	pushNumber (data -> v_getYmax ());
}
static void do_object_nx () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].nx");
	Melder_require (data -> v_hasGetNx (),
		U"An object of type ", Thing_className (data), U" has no “nx” attribute.");
	pushNumber (data -> v_getNx ());
}
static void do_object_ny () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].ny");
	Melder_require (data -> v_hasGetNy (),
		U"An object of type ", Thing_className (data), U" has no “ny” attribute.");
	pushNumber (data -> v_getNy ());
}
static void do_object_dx () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].dx");
	Melder_require (data -> v_hasGetDx (),
		U"An object of type ", Thing_className (data), U" has no “dx” attribute.");
	pushNumber (data -> v_getDx ());
}
static void do_object_dy () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].dy");
	Melder_require (data -> v_hasGetDy (),
		U"An object of type ", Thing_className (data), U" has no “dy” attribute.");
	pushNumber (data -> v_getDy ());
}
static void do_object_nrow () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].nrow");
	Melder_require (data -> v_hasGetNrow (),
		U"An object of type ", Thing_className (data), U" has no “nrow” attribute.");
	pushNumber (data -> v_getNrow ());
}
static void do_object_ncol () {
	const Stackel object = pop;
	const Daata data = _do_object (object, U"object[xx].ncol");
	Melder_require (data -> v_hasGetNcol (),
		U"An object of type ", Thing_className (data), U" has no “ncol” attribute.");
	pushNumber (data -> v_getNcol ());
}
static void do_object_row_STR () {
	const Stackel index = pop, object = pop;
	const Daata data = _do_object (object, U"object[xx].row$[]");
	Melder_require (data -> v_hasGetRowStr (),
		U"An object of type ", Thing_className (data), U" has no “row$[]” attribute.");
	Melder_require (index -> which == Stackel_NUMBER,
		U"The expression “object[].row$[xx]” requires xx to be a number, not ", index->whichText(), U".");
	const integer number = Melder_iround (index->number);
	autostring32 result = Melder_dup (data -> v_getRowStr (number));
	if (! result)
		Melder_throw (U"Row index out of bounds.");
	pushString (result.move());
}
static void do_object_col_STR () {
	const Stackel index = pop, object = pop;
	const Daata data = _do_object (object, U"object[xx].col$[]");
	Melder_require (data -> v_hasGetColStr (),
		U"An object of type ", Thing_className (data), U" has no “col$[]” attribute.");
	Melder_require (index -> which == Stackel_NUMBER,
		U"The expression “object[].col$[xx]” requires xx to be a number, not ", index->whichText(), U".");
	const integer number = Melder_iround (index->number);
	autostring32 result = Melder_dup (data -> v_getColStr (number));
	if (! result)
		Melder_throw (U"Column index out of bounds.");
	pushString (result.move());
}
static void do_string_STR () {
	const Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_double (value->number));
		pushString (result.move());
	} else {
		Melder_throw (U"The function “string$” requires a number, not ", value->whichText(), U".");
	}
}
static void do_vertical_STR () {
	const Stackel array = pop;
	if (array->which == Stackel_STRING_ARRAY) {
		pushString (Melder_dup (Melder_STRVEC (array->stringArray)));
	} else {
		Melder_throw (U"The function “vertical$” requires a string array.");
	}
}
static void do_numbers_VEC () {
	/*
		result# = numbers# (strings$#)
	*/
	const Stackel stringsel = pop;
	if (stringsel->which == Stackel_STRING_ARRAY) {
		const constSTRVEC strings = stringsel->stringArray;
		autoVEC result = zero_VEC (strings.size);
		for (integer i = 1; i <= strings.size; i ++)
			result [i] = Melder_atof (strings [i]);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “numbers#” requires a string array, not ", stringsel->whichText(), U".");
	}
}
static void do_sleep () {
	const Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		Melder_sleep (value->number);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “sleep” requires a number, not ", value->whichText(), U".");
	}
}
static void do_unicode () {
	const Stackel value = pop;
	if (value->which == Stackel_STRING) {
		pushNumber (value->getString() [0]);
	} else {
		Melder_throw (U"The function “unicode” requires a character, not ", value->whichText(), U".");
	}
}
static void do_unicode_STR () {
	const Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		Melder_require (value->number >= 0.0 && value->number < (double) (1 << 21),
			U"A unicode number cannot be greater than ", (1 << 21) - 1, U".");
		Melder_require (value->number < 0xD800 || value->number > 0xDFFF,
			U"A unicode number cannot lie between 0xD800 and 0xDFFF. Those are “surrogates”.");
		char32 string [2] = { U'\0', U'\0' };
		string [0] = (char32) value->number;
		pushString (Melder_dup (string).move());
	} else {
		Melder_throw (U"The function “unicode$” requires a number, not ", value->whichText(), U".");
	}
}
static void do_fixed_STR () {
	const Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_fixed (value->number, Melder_iround (precision->number)));
		pushString (result.move());
	} else if (value->which == Stackel_NUMERIC_VECTOR && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_fixed (value->numericVector, Melder_iround (precision->number)));
		pushString (result.move());
	} else if (value->which == Stackel_NUMERIC_MATRIX && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_fixed (value->numericMatrix, Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function “fixed$” requires a number or a vector or a matrix, and a number (the precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_percent_STR () {
	const Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_percent (value->number, Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function “percent$” requires two numbers (value and precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_hexadecimal_STR () {
	const Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_hexadecimal (Melder_iround (value->number), Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function “hexadecimal$” requires two numbers (value and precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_deleteFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “deleteFile” is not available inside manuals.");
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		MelderFile_delete (& file);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “deleteFile” requires a string, not ", f->whichText(), U".");
	}
}
static void do_moveAndOrRenameFile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “moveAndOrRenameFile” is not available inside manuals.");
	const Stackel to = pop, from = pop;
	if (from->which == Stackel_STRING && to->which == Stackel_STRING) {
		structMelderFile fromFile { }, toFile { };
		Melder_relativePathToFile (from->getString(), & fromFile);
		Melder_relativePathToFile (to->getString(), & toFile);
		MelderFile_moveAndOrRename (& fromFile, & toFile);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “moveAndOrRenameFile” requires two strings, not ", from->whichText(), U" and ", to->whichText(), U".");
	}
}
static void do_createFolder () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “createFolder” is not available inside manuals.");
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFolder folder { };
		Melder_relativePathToFolder (f->getString(), & folder);
		MelderFolder_create (& folder);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “createFolder” requires a string, not ", f->whichText(), U".");
	}
}
static void do_createDirectory () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “createDirectory” is not available inside manuals.");
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFolder folder { };
		Melder_relativePathToFolder (f->getString(), & folder);
		MelderFolder_create (& folder);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “createDirectory” requires a string, not ", f->whichText(), U".");
	}
}
static void do_setWorkingDirectory () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFolder folder { };
		Melder_relativePathToFolder (f->getString(), & folder);
		Melder_setCurrentFolder (& folder);
		MelderFolder_copy (& folder, & theInterpreter -> workingDirectory);
		Melder_assert (MelderFolder_equal (Melder_peekWorkingDirectory(), & theInterpreter -> workingDirectory));
		//TRACE
		trace (U"Set the working directory to ", Melder_peekWorkingDirectory());
		trace (U"Set the working directory of interpreter ", Melder_pointer (theInterpreter), U" to ", & theInterpreter -> workingDirectory);
		pushNumber (1);
	} else {
		Melder_throw (U"The function “setWorkingDirectory” requires a string, not ", f->whichText(), U".");
	}
}
static void do_variableExists () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		bool result = !! Interpreter_hasVariable (theInterpreter, f->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function “variableExists” requires a string, not ", f->whichText(), U".");
	}
}
static void do_readFile () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushNumber (Melder_atof (text.get()));
	} else {
		Melder_throw (U"The function “readFile” requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_readFile_STR () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushString (text.move());
	} else {
		Melder_throw (U"The function “readFile$” requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_readFile_VEC () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushNumericVector (splitByWhitespace_VEC (text.get()));
	} else {
		Melder_throw (U"The function “readFile#” requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_readFile_MAT () {
	const Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushNumericMatrix (splitByLinesAndWhitespace_MAT (text.get()));
	} else {
		Melder_throw (U"The function “readFile##” requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_tensorLiteral () {
	const Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	integer numberOfElements = Melder_iround (n->number);
	Melder_assert (numberOfElements >= 0);
	if (numberOfElements == 0) {
		autoVEC result = raw_VEC (0);
		pushNumericVector (result.move());
		return;
	}
	/*
		The type of the tensor can be a vector, or a matrix, or a tensor3...
		This depends on whether the last element is a number, a vector, or a matrix...
	*/
	const Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		autoVEC result = raw_VEC (numberOfElements);
		result [numberOfElements] = last->number;
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			const Stackel element = pop;
			if (element->which != Stackel_NUMBER)
				Melder_throw (U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a number.");
			result [ielement] = element->number;
		}
		pushNumericVector (result.move());
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			a# = zero# (0)   ; edge case
			a## = { a#, a# }
			assert numberOfRows (a##) = 2
			assert numberOfColumns (a##) = 0
		@*/
		const integer sharedNumberOfColumns = last->numericVector.size;
		autoMAT result = raw_MAT (numberOfElements, sharedNumberOfColumns);
		result.row (numberOfElements)  <<=  last->numericVector;
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_NUMERIC_VECTOR,
				U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a vector.");
			Melder_require (element->numericVector.size == sharedNumberOfColumns,
				U"The vectors have to be of the same size, not ", element->numericVector.size, U" and ", sharedNumberOfColumns);
			result.row (ielement)  <<=  element->numericVector;
		}
		pushNumericMatrix (result.move());
	} else if (last->which == Stackel_STRING) {
		autoSTRVEC result (numberOfElements);
		result [numberOfElements] = last->_string.move();
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			const Stackel element = pop;
			Melder_require (element->which == Stackel_STRING,
				U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a string.");
			result [ielement]  =  element->_string.move();
		}
		pushStringVector (std::move (result));
	} else {
		Melder_throw (U"Cannot (yet?) create a tensor containing ", last->whichText(), U".");
	}
}
static void do_tensorLiteralCell () {
	const Stackel index = pop, nelem = pop;
	Melder_assert (nelem->which == Stackel_NUMBER);
	const integer numberOfElements = Melder_iround (nelem->number);
	Melder_assert (numberOfElements > 0);
	const Stackel last = pop;
	Melder_require (last->which == Stackel_NUMBER || last->which == Stackel_STRING,
		U"Cannot index matrix literals.");
	Melder_require (index->which == Stackel_NUMBER,
		U"In vector indexing, the element index should be a number, not ", index->whichText(), U".");
	Melder_require (isdefined (index->number),
		U"The element index is undefined.");
	const integer iindex = Melder_iround (index->number);
	Melder_require (iindex >= 1,
		U"Element index out of bounds. Your index is ", iindex, U", but the index can range only from 1 to ", numberOfElements, U".");
	Melder_require (iindex <= numberOfElements,
		U"Element index out of bounds. Your index is ", iindex, U", but there are only ", numberOfElements, U" elements.");
	if (last->which == Stackel_NUMBER) {
		double result = last->number;
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			const Stackel element = pop;
			if (element->which != Stackel_NUMBER)
				Melder_throw (U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a number.");
			if (ielement == iindex)
				result = element->number;
		}
		pushNumber (result);
	} else if (last->which == Stackel_STRING) {
		autostring32 result = last->_string.move();   // we can move it, because it's been popped off the stack, so we never return there
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			const Stackel element = pop;
			if (element->which != Stackel_STRING)
				Melder_throw (U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a string.");
			if (ielement == iindex)
				result = element->_string.move();   // we can move it, because it's been popped off the stack, so we never return there
		}
		pushString (result.move());
	}
}
static void do_row_VEC () {
	/*
		result# = row# (mat##, rowNumber)
	*/
	const Stackel rowNumber_ = pop, mat_ = pop;
	if (mat_->which == Stackel_NUMERIC_MATRIX && rowNumber_->which == Stackel_NUMBER) {
		const constMAT mat = mat_->numericMatrix;
		const integer rowNumber = Melder_iround (rowNumber_->number);
		Melder_require (rowNumber >= 1 && rowNumber <= mat.nrow,
			U"In the function “row#”, the row number should be between 1 and ", mat.nrow, U".");
		autoVEC result = copy_VEC (mat.row (rowNumber));
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “row#” requires a matrix and a row number, not ", mat_->whichText(), U" and ", rowNumber_->whichText(), U".");
	}
}
static void do_col_VEC () {
	/*
		result# = col# (mat##, colNumber)
	*/
	const Stackel colNumber_ = pop, mat_ = pop;
	if (mat_->which == Stackel_NUMERIC_MATRIX && colNumber_->which == Stackel_NUMBER) {
		const constMAT mat = mat_->numericMatrix;
		const integer colNumber = Melder_iround (colNumber_->number);
		Melder_require (colNumber >= 1 && colNumber <= mat.ncol,
			U"In the function “col#”, the column number should be between 1 and ", mat.ncol, U".");
		autoVEC result = copy_VEC (mat.column (colNumber));
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “col#” requires a matrix and a column number, not ", mat_->whichText(), U" and ", colNumber_->whichText(), U".");
	}
}
static void do_inner () {
	/*
		result = inner (x#, y#)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMinner (x->numericVector, y->numericVector));
	} else {
		Melder_throw (U"The function “inner” requires two vectors, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_correlation () {
	/*
		result = correlation (x#, y#)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMcorrelation (x->numericVector, y->numericVector));
	} else {
		Melder_throw (U"The function “correlation” requires two vectors, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_outer_MAT () {
	/*
		result## = outer## (x#, y#)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		autoMAT result = outer_MAT (x->numericVector, y->numericVector);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “outer##” requires two vectors, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_VEC () {
	/*
		result# = mul# (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul# (x#, y##)
		*/
		const integer xSize = x->numericVector.size, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xSize,
			U"In the function “mul#”, the dimension of the vector and the number of rows of the matrix should be equal, "
			U"not ", xSize, U" and ", yNrow
		);
		autoVEC result = mul_VEC (x->numericVector, y->numericMatrix);
		pushNumericVector (result.move());
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR) {
		/*
			result# = mul# (x##, y#)
		*/
		const integer xNcol = x->numericMatrix.ncol, ySize = y->numericVector.size;
		Melder_require (ySize == xNcol,
			U"In the function “mul#”, the number of columns of the matrix and the dimension of the vector should be equal, "
			U"not ", xNcol, U" and ", ySize, U"."
		);
		autoVEC result = mul_VEC (x->numericMatrix, y->numericVector);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “mul#” requires a vector and a matrix, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_MAT () {
	/*
		result## = mul## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul## (x##, y##)
		*/
		const integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function “mul##”, the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = zero_MAT (x->numericMatrix.nrow, y->numericMatrix.ncol);
		_mul_allowAllocation_MAT_out (result.get(), x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_metal_MAT () {
	/*
		result## = mul## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul## (x##, y##)
		*/
		const integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function “mul##”, the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = zero_MAT (x->numericMatrix.nrow, y->numericMatrix.ncol);
		MATmul_forceMetal_ (result.get(), x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul_metal##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_fast_MAT () {
	/*
		result## = mul_fast## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_fast## (x##, y##)
		*/
		const integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function “mul_fast##”, the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = mul_fast_MAT (x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul_fast##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_tn_MAT () {
	/*
		result## = mul_tn## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_tn## (x##, y##)
		*/
		const integer xNrow = x->numericMatrix.nrow, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNrow,
			U"In the function “mul_tn##”, the number of rows of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNrow, U" and ", yNrow, U"."
		);
		autoMAT result = mul_allowAllocation_MAT (x->numericMatrix.transpose(), y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul_tn##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_nt_MAT () {
	/*
		result## = mul_nt## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_nt## (x##, y##)
		*/
		const integer xNcol = x->numericMatrix.ncol, yNcol = y->numericMatrix.ncol;
		Melder_require (yNcol == xNcol,
			U"In the function “mul_tn##”, the number of columns of the first matrix and the number of columns of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNcol, U"."
		);
		autoMAT result = mul_allowAllocation_MAT (x->numericMatrix, y->numericMatrix.transpose());
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul_nt##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_mul_tt_MAT () {
	/*
		result## = mul_tt## (x.., y..)
	*/
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_tt## (x##, y##)
		*/
		const integer xNrow = x->numericMatrix.nrow, yNcol = y->numericMatrix.ncol;
		Melder_require (yNcol == xNrow,
			U"In the function “mul_tt##”, the number of rows of the first matrix and the number of columns of the second matrix should be equal, "
			U"not ", xNrow, U" and ", yNcol, U"."
		);
		autoMAT result = mul_allowAllocation_MAT (x->numericMatrix.transpose(), y->numericMatrix.transpose());
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function “mul_tt##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_transpose_MAT () {
	const Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (x->owned) {
			if (NUMisSquare (x->numericMatrix)) {
				/*@praat
					assert transpose## ({ { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } }) = { { 1, 4, 7 }, { 2, 5, 8 }, { 3, 6, 9 } }
				@*/
				transpose_mustBeSquare_MAT_inout (x->numericMatrix);
			} else {
				/*@praat
					assert transpose## ({ { 1, 2, 3 }, { 4, 5, 6 } }) = { { 1, 4 }, { 2, 5 }, { 3, 6 } }
				@*/
				autoMAT newMatrix = transpose_MAT (x->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix.releaseToAmbiguousOwner();
			}
		} else {
			/*@praat
				a## = { { 1, 2, 3 }, { 4, 5, 6 } }
				assert transpose## (a##) = { { 1, 4 }, { 2, 5 }, { 3, 6 } }
				assert transpose## (transpose## (a##)) = a##
			@*/
			x->numericMatrix = transpose_MAT (x->numericMatrix). releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else {
		Melder_throw (U"The function “transpose##” requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_rowSums_VEC () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		autoVEC result = rowSums_VEC (x->numericMatrix);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “rowSums#” requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_columnSums_VEC () {
	const Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		/*@praat
			a## = { { 4, 7, -10 }, { 16, 0, 88 } }
			result# = columnSums# (a##)
			assert result# = { 20, 7, 78 }
		@*/
		autoVEC result = columnSums_VEC (x->numericMatrix);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “columnSums#” requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_repeat_VEC () {
	const Stackel n = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && n->which == Stackel_NUMBER) {
		const integer n_old = x->numericVector.size;
		const integer times = Melder_iround (n->number);
		autoVEC result = raw_VEC (n_old * times);
		for (integer i = 1; i <= times; i ++)
			for (integer j = 1; j <= n_old; j ++)
				result [(i - 1) * n_old + j] = x->numericVector [j];
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function “repeat#” requires a vector and a number, not ", x->whichText(), U" and ", n->whichText(), U".");
	}
}
static void do_rowInners_VEC () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		Melder_require (x->numericMatrix.nrow == y->numericMatrix.nrow && x->numericMatrix.ncol == y->numericMatrix.ncol,
			U"In the function rowInners#, the two matrices should have the same shape, not ",
			x->numericMatrix.nrow, U"x", x->numericMatrix.ncol, U" and ", y->numericMatrix.nrow, U"x", y->numericMatrix.ncol
		);
		pushNumericVector (rowInners_VEC (x->numericMatrix, y->numericMatrix));
	} else {
		Melder_throw (U"The function “rowInners#” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_solve_VEC () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR) {
		Melder_require (x->numericMatrix.nrow == y->numericVector.size,
			U"In the function “solve#”, the number of rows of the matrix and the dimension of the vector should be equal, not ",
			x->numericMatrix.nrow, U" and ", y->numericVector.size
		);
		pushNumericVector (solve_VEC (x->numericMatrix, y->numericVector, NUMeps * y->numericVector.size));
	} else {
		Melder_throw (U"The function “solve#” requires a matrix and a vector, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}

static void do_solveWeaklyConstrained_VEC () {
	const Stackel delta = pop, alpha = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR && alpha->which == Stackel_NUMBER && delta->which == Stackel_NUMBER) {
		Melder_require (x->numericMatrix.nrow == y->numericVector.size,
			U"In the function “solveWeaklyConstrained#”, the number of rows of the matrix and the dimension of the vector should be equal, not ",
			x->numericMatrix.nrow, U" and ", y->numericVector.size
		);
		Melder_require (alpha->number >= 0.0,
			U"Argument 3, the weight coefficient of the penalty function should not be negative.");
		Melder_require (delta->number >= 0.0,
			U"Argument 4, the squared length of the solution vector should not be negative.");
		pushNumericVector (solveWeaklyConstrainedLinearRegression_VEC (x->numericMatrix, y->numericVector, alpha->number, delta->number));
	} else {
		Melder_throw (U"The function “solveWeaklyConstrained#” requires a matrix, a vector, and two numbers not ", x->whichText(), U", ",
			y->whichText(), U", ", alpha->whichText(), U" and ", delta->whichText(), U".");
	}
}

static void do_solve_MAT () {
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		Melder_require (x->numericMatrix.nrow == y->numericMatrix.nrow,
			U"In the function “solve##”, the two matrices should have the same number of rows, not ",
			x->numericMatrix.nrow, U" and ", y->numericMatrix.nrow);
		pushNumericMatrix (solve_MAT (x->numericMatrix, y->numericMatrix, NUMeps * x->numericMatrix.nrow * x->numericMatrix.ncol));
	} else {
		Melder_throw (U"The function “solve##” requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}

static void do_solveSparse_VEC () {
	const Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number == 6) {
		const Stackel info = pop, tol = pop, niter = pop, nonzeros = pop, y = pop, dict = pop;
		if (dict->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR &&
			nonzeros->which == Stackel_NUMBER && niter->which == Stackel_NUMBER &&
			tol->which == Stackel_NUMBER && info->which == Stackel_NUMBER) {
			const MAT d = dict->numericMatrix;
			Melder_require (d.ncol > d.nrow,
				U"The number of columns should be larger than the number of rows (sparse!).");
			const VEC yy = y->numericVector;
			Melder_require (yy.size == d.nrow,
				U"The number of rows in the matrix should equal the size of the vector.");
			const integer numberOfNonzeros = Melder_iround (nonzeros->number);
			const integer maximumNumberOfIterations = Melder_iround (niter ->number);
			const integer infoLevel = Melder_iround (info->number);
			const double tolerance = tol->number;
			pushNumericVector (solveSparse_IHT_VEC (d, yy, numberOfNonzeros,  maximumNumberOfIterations, tolerance, infoLevel));
		} else {
			Melder_throw (U"The function “solveSparse#” requires a matrix, a vector, and four numbers, not ", dict->whichText(), U", ", y->whichText(), U", ", nonzeros->whichText(), U", ",
			niter->whichText(), U", ", tol->whichText(), U" and ", info->whichText());
		}
	} else if (n -> number == 7) {
		const Stackel info = pop, tol = pop, niter = pop, nonzeros = pop, xstart = pop, y = pop, dict = pop;
		if (dict->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR &&
			xstart->which == Stackel_NUMERIC_VECTOR &&
			nonzeros->which == Stackel_NUMBER && niter->which == Stackel_NUMBER &&
			tol->which == Stackel_NUMBER && info->which == Stackel_NUMBER)
		{
			const constMAT d = dict->numericMatrix;
			Melder_require (d.ncol > d.nrow,
				U"The number of columns should be larger than the number of rows (sparse!).");
			const constVEC yy = y->numericVector;
			const VEC x = xstart->numericVector;
			Melder_require (yy.size == d.nrow,
				U"The number of rows in the matrix should equal the size of the vector.");
			Melder_require (x.size == d.ncol,
				U"The number of columns in the matrix should equal the size of start vector.");
			autoVEC xs = copy_VEC (x);
			const integer numberOfNonzeros = Melder_iround (nonzeros->number);
			const integer maximumNumberOfIterations = Melder_iround (niter ->number);
			const integer infoLevel = Melder_iround (info->number);
			const double tolerance = tol->number;
			VECsolveSparse_IHT (xs.all(), d, yy, numberOfNonzeros, maximumNumberOfIterations, tolerance, infoLevel);
			pushNumericVector (xs.move());
		} else {
			Melder_throw (U"The function “solveSparse#” requires a matrix, a vector, and four numbers, not ", dict->whichText(), U", ", y->whichText(), U", ", nonzeros->whichText(), U", ",
			niter->whichText(), U", ", tol->whichText(), U" and ", info->whichText());
		}
	} else {
		Melder_throw (U"The function “solveSparse#” requires six or seven arguments.");
	}
}

static void do_solveNonnegative_VEC () {
	const Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number == 6) {
		const Stackel info = pop, tol = pop, itermax = pop, xstart = pop, y = pop, m = pop;
		if (m->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR &&
			xstart->which == Stackel_NUMERIC_VECTOR && itermax->which == Stackel_NUMBER &&
			tol->which == Stackel_NUMBER && info->which == Stackel_NUMBER)
		{
			const double tolerance = tol->number;
			const integer maximumNumberOfIterations = Melder_iround (itermax->number);
			const integer infoLevel = Melder_iround (info->number);
			const constMAT a = m->numericMatrix;
			const VEC yy = y->numericVector;
			Melder_require (a.nrow == yy.size,
				U"The number of rows in the matrix should equal the size of the vector.");
			autoVEC x = copy_VEC (xstart->numericVector);
			Melder_require (x.size == a.ncol,
				U"The size of start vector should equal the number of columns in the matrix.");
			VECsolveNonnegativeLeastSquaresRegression (x.get(), a, yy, maximumNumberOfIterations, tolerance, infoLevel);
			pushNumericVector (x.move());
		} else {
			Melder_throw (U"The function “solveNonnegative#” requires a matrix, a vector, a vector and three numbers, not ", m->whichText(), U", ", y->whichText(), U", ", xstart->whichText(), U", ",
			itermax->whichText(), U", ", tol->whichText(), U" and ", info->whichText());
		}
	} else if (n -> number == 5) {
		const Stackel info = pop, tol = pop, itermax = pop, y = pop, m = pop;
		if (m->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR &&
			itermax->which == Stackel_NUMBER && tol->which == Stackel_NUMBER && info->which == Stackel_NUMBER)
		{
			const double tolerance = tol->number;
			const integer maximumNumberOfIterations = Melder_iround (itermax->number);
			const integer infoLevel = Melder_iround (info->number);
			const constMAT a = m->numericMatrix;
			const VEC yy = y->numericVector;
			Melder_require (a.nrow == yy.size,
				U"The number of rows in the matrix should equal the size of the vector.");
			pushNumericVector (solveNonnegativeLeastSquaresRegression_VEC (a, yy, maximumNumberOfIterations, tolerance, infoLevel));
		} else {
			Melder_throw (U"The function “solveNonnegative#” requires a matrix, a vector, and three numbers, not ", m->whichText(), U", ", y->whichText(), U", ", itermax->whichText(), U", ", tol->whichText(), U" and ", info->whichText());
		}
	} else {
		Melder_throw (U"The function “solveNonnegative#” requires five or six arguments.");
	}
}

static void do_askForTrust () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “askForTrust” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “askForTrust” should have no arguments.");
	const Editor optionalTrustWindowOwningEditor = theInterpreter -> optionalDynamicEnvironmentEditor();
	const GuiWindow parentShell = ( optionalTrustWindowOwningEditor ? optionalTrustWindowOwningEditor -> windowForm : theCurrentPraatApplication -> topShell );
	const bool isAlreadyTrusted = theInterpreter -> scriptReference && theInterpreter -> scriptReference -> trusted;
	if (! isAlreadyTrusted) {
		conststring32 message [1+5] = { };
		if (theInterpreter -> scriptReference) {
			message [1] = U"The script";
			message [2] = Melder_cat (U"“", theInterpreter -> scriptReference -> string.get(), U"”");
		} else if (theInterpreter -> notebookReference) {
			message [1] = U"The notebook";
			message [2] = Melder_cat (U"“", theInterpreter -> notebookReference -> string.get(), U"”");
		} else
			message [1] =  U"Your untitled script or notebook";
		message [3] = U"requests permission to control your computer (e.g. it may want to overwrite files,\n"
			"delete folders, run system commands, and/or access the internet).";
		message [4] = U"Allow this only if you fully trust the intentions and skills of the author(s).";
		conststring32 option1 = U"CANCEL\n(because I don’t completely trust the authors’ skills and/or intentions)";
		conststring32 option2 =
			theInterpreter -> scriptReference ?
				U"Yes, I allow this script to CONTROL MY COMPUTER\n(because I fully trust its authors’ skills and intentions)"
			: theInterpreter -> notebookReference ?
				U"Yes, I allow this notebook to CONTROL MY COMPUTER,\n(because I fully trust its authors’ skills and intentions)"
			:
				U"Yes, I allow this script or notebook to CONTROL MY COMPUTER,\n(because I fully trust its authors’ skills and intentions)";
		const bool trusted = GuiTrust_get (parentShell, optionalTrustWindowOwningEditor,
			message [1], message [2], message [3], message [4], message [5],
			option1, option2, nullptr, nullptr, nullptr, theInterpreter
		);
		if (trusted) {
			if (theInterpreter -> scriptReference)
				theInterpreter -> scriptReference -> trusted = true;
			else if (theInterpreter -> notebookReference)
				theInterpreter -> notebookReference -> trusted = true;
		}
	}
	pushNumber (1);
}

static void do_beginPause () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “beginPause” is not available inside manuals.");
	const Stackel n = pop;
	if (n->number == 1) {
		const Stackel title = pop;
		if (title->which == Stackel_STRING) {
			const Editor optionalPauseWindowOwningEditor = theInterpreter -> optionalDynamicEnvironmentEditor();
			const GuiWindow parentShell = ( optionalPauseWindowOwningEditor ? optionalPauseWindowOwningEditor -> windowForm : theCurrentPraatApplication -> topShell );
			UiPause_begin (parentShell, optionalPauseWindowOwningEditor, title->getString(), theInterpreter);
		} else {
			Melder_throw (U"The function “beginPause” requires a string (the title), not ", title->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “beginPause” requires 1 argument (a title), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_real () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “real” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “real” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	conststring32 defaultString = nullptr;
	if (defaultValue->which == Stackel_STRING)
		defaultString = defaultValue->getString();
	else if (defaultValue->which == Stackel_NUMBER)
		defaultString = Melder_double (defaultValue->number);
	else
		Melder_throw (U"The second argument of “real” (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “real” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_real (label->getString(), defaultString);
	pushNumber (1);
}
static void do_positive () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “positive” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “positive” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	conststring32 defaultString = nullptr;
	if (defaultValue->which == Stackel_STRING) {
		defaultString = defaultValue->getString();
	} else if (defaultValue->which == Stackel_NUMBER) {
		defaultString = Melder_double (defaultValue->number);
	} else {
		Melder_throw (U"The second argument of “positive” (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
	}
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “positive” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_positive (label->getString(), defaultString);
	pushNumber (1);
}
static void do_integer () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “integer” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “integer” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	conststring32 defaultString = nullptr;
	if (defaultValue->which == Stackel_STRING) {
		defaultString = defaultValue->getString();
	} else if (defaultValue->which == Stackel_NUMBER) {
		defaultString = Melder_double (defaultValue->number);
	} else {
		Melder_throw (U"The second argument of “integer” (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
	}
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “integer” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_integer (label->getString(), defaultString);
	pushNumber (1);
}
static void do_natural () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “natural” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “natural” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	conststring32 defaultString = nullptr;
	if (defaultValue->which == Stackel_STRING) {
		defaultString = defaultValue->getString();
	} else if (defaultValue->which == Stackel_NUMBER) {
		defaultString = Melder_double (defaultValue->number);
	} else {
		Melder_throw (U"The second argument of “natural” (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
	}
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “natural” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_natural (label->getString(), defaultString);
	pushNumber (1);
}
static void do_word () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “word” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “word” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The second argument of “word” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “word” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_word (label->getString(), defaultValue->getString());
	pushNumber (1);
}
static void do_sentence () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “sentence” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “sentence” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The second argument of “sentence” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “sentence” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_sentence (label->getString(), defaultValue->getString());
	pushNumber (1);
}
static void do_text () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “text” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 2 && n->number <= 3,
		U"The function “text” requires 2 or 3 arguments (an optional number of lines, a label text, and a default value), not ", n->number, U".");
	integer numberOfLines = 1;   // the default
	if (n->number == 3) {
		/*
			For compatibility, the number of lines might be at the end.
		*/
		const Stackel arg3 = pop, arg2 = pop, arg1 = pop;
		if (arg1->which == Stackel_STRING && arg2->which == Stackel_STRING && arg3->which == Stackel_NUMBER) {
			UiPause_text (arg1->getString(), arg2->getString(), Melder_iround (arg3->number));
			pushNumber (1);
			return;
		}
		const Stackel defaultValue = arg3;
		Melder_require (defaultValue->which == Stackel_STRING,
			U"The third argument of “text” (the default value) should be a string, not ", defaultValue->whichText(), U".");
		const Stackel label = arg2;
		Melder_require (label->which == Stackel_STRING,
			U"The second argument of “text” (the label) should be a string, not ", label->whichText(), U".");
		const Stackel _numberOfLines = arg1;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “text” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
		UiPause_text (label->getString(), defaultValue->getString(), numberOfLines);
		pushNumber (1);
		return;
	}
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The second argument of “text” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “text” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_text (label->getString(), defaultValue->getString(), numberOfLines);
	pushNumber (1);
}
static void do_infile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “infile” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 2 && n->number <= 3,
		U"The function “infile” requires 2 or 3 arguments (an optional number of lines, a label text, and a default value), not ", n->number, U".");
	integer numberOfLines = 3;   // the default
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The last argument of “infile” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The penultimate argument of “infile” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 3) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “infile” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_infile (label->getString(), defaultValue->getString(), numberOfLines);
	pushNumber (1);
}
static void do_outfile () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “outfile” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 2 && n->number <= 3,
		U"The function “outfile” requires 2 or 3 arguments (an optional number of lines, a label text, and a default value), not ", n->number, U".");
	integer numberOfLines = 3;   // the default
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The last argument of “outfile” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The penultimate argument of “outfile” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 3) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “outfile” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_outfile (label->getString(), defaultValue->getString(), numberOfLines);
	pushNumber (1);
}
static void do_folder () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “folder” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 2 && n->number <= 3,
		U"The function “folder” requires 2 or 3 arguments (an optional number of lines, a label text, and a default value), not ", n->number, U".");
	integer numberOfLines = 3;   // the default
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_STRING,
		U"The last argument of “folder” (the default value) should be a string, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The penultimate argument of “folder” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 3) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “folder” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_folder (label->getString(), defaultValue->getString(), numberOfLines);
	pushNumber (1);
}
static void do_realvector () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “realvector” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 3 && n->number <= 4,
		U"The function “realvector” requires 3 or 4 arguments (an optional number of lines, a label text, a format, and a default value), not ", n->number, U".");
	integer numberOfLines = 7;   // the default
	const Stackel defaultValue = pop;
	conststring32 defaultString;
	if (defaultValue->which == Stackel_STRING)
		defaultString = defaultValue->getString();
	else if (defaultValue->which == Stackel_NUMERIC_VECTOR)
		defaultString = Melder_VEC (defaultValue->numericVector, true);
	else
		Melder_throw (U"The last argument of “realvector” (the default value) should be a string or a numeric vector, not ", defaultValue->whichText(), U".");
	const Stackel _format = pop;
	Melder_require (_format->which == Stackel_STRING,
		U"The penultimate argument of “realvector” (the format) should be a string, not ", _format->whichText(), U".");
	kUi_realVectorFormat format = kUi_realVectorFormat_getValue (_format->getString());
	Melder_require (format != kUi_realVectorFormat::UNDEFINED,
		U"The format should be “(whitespace-separated)” or “(formula)”, not “", _format->getString(), U"”.");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The antepenultimate argument of “realvector” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 4) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “realvector” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_realvector (label->getString(), format, defaultString, numberOfLines);
	pushNumber (1);
}
static void do_positivevector () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “positivevector” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 3 && n->number <= 4,
		U"The function “positivevector” requires 3 or 4 arguments (an optional number of lines, a label text, a format, and a default value), not ", n->number, U".");
	integer numberOfLines = 7;   // the default
	const Stackel defaultValue = pop;
	conststring32 defaultString;
	if (defaultValue->which == Stackel_STRING)
		defaultString = defaultValue->getString();
	else if (defaultValue->which == Stackel_NUMERIC_VECTOR)
		defaultString = Melder_VEC (defaultValue->numericVector, true);
	else
		Melder_throw (U"The last argument of “positivevector” (the default value) should be a string or a numeric vector, not ", defaultValue->whichText(), U".");
	const Stackel _format = pop;
	Melder_require (_format->which == Stackel_STRING,
		U"The penultimate argument of “positivevector” (the format) should be a string, not ", _format->whichText(), U".");
	kUi_realVectorFormat format = kUi_realVectorFormat_getValue (_format->getString());
	Melder_require (format != kUi_realVectorFormat::UNDEFINED,
		U"The format should be “(whitespace-separated)” or “(formula)”, not “", _format->getString(), U"”.");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The antepenultimate argument of “positivevector” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 4) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “positivevector” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_positivevector (label->getString(), format, defaultString, numberOfLines);
	pushNumber (1);
}
static void do_integervector () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “integervector” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 3 && n->number <= 4,
		U"The function “integervector” requires 3 or 4 arguments (an optional number of lines, a label text, a format, and a default value), not ", n->number, U".");
	integer numberOfLines = 7;   // the default
	const Stackel defaultValue = pop;
	conststring32 defaultString;
	if (defaultValue->which == Stackel_STRING)
		defaultString = defaultValue->getString();
	else if (defaultValue->which == Stackel_NUMERIC_VECTOR)
		defaultString = Melder_VEC (defaultValue->numericVector, true);
	else
		Melder_throw (U"The last argument of “integervector” (the default value) should be a string or a numeric vector, not ", defaultValue->whichText(), U".");
	const Stackel _format = pop;
	Melder_require (_format->which == Stackel_STRING,
		U"The penultimate argument of “integervector” (the format) should be a string, not ", _format->whichText(), U".");
	kUi_integerVectorFormat format = kUi_integerVectorFormat_getValue (_format->getString());
	Melder_require (format != kUi_integerVectorFormat::UNDEFINED,
		U"The format should be “(whitespace-separated)”, “(ranges)”, or “(formula)”, not “", _format->getString(), U"”.");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The antepenultimate argument of “integervector” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 4) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “integervector” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_integervector (label->getString(), format, defaultString, numberOfLines);
	pushNumber (1);
}
static void do_naturalvector () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “naturalvector” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number >= 3 && n->number <= 4,
		U"The function “naturalvector” requires 3 or 4 arguments (an optional number of lines, a label text, a format, and a default value), not ", n->number, U".");
	integer numberOfLines = 7;   // the default
	const Stackel defaultValue = pop;
	conststring32 defaultString;
	if (defaultValue->which == Stackel_STRING)
		defaultString = defaultValue->getString();
	else if (defaultValue->which == Stackel_NUMERIC_VECTOR)
		defaultString = Melder_VEC (defaultValue->numericVector, true);
	else
		Melder_throw (U"The last argument of “naturalector” (the default value) should be a string or a numeric vector, not ", defaultValue->whichText(), U".");
	const Stackel _format = pop;
	Melder_require (_format->which == Stackel_STRING,
		U"The penultimate argument of “naturalvector” (the format) should be a string, not ", _format->whichText(), U".");
	kUi_integerVectorFormat format = kUi_integerVectorFormat_getValue (_format->getString());
	Melder_require (format != kUi_integerVectorFormat::UNDEFINED,
		U"The format should be “(whitespace-separated)”, “(ranges)”, or “(formula)”, not “", _format->getString(), U"”.");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The antepenultimate argument of “naturalvector” (the label text) should be a string, not ", label->whichText(), U".");
	if (n->number == 4) {
		const Stackel _numberOfLines = pop;
		Melder_require (_numberOfLines->which == Stackel_NUMBER,
			U"The first argument of “naturalvector” (the number of lines) should be a number, not ", _numberOfLines->whichText(), U".");
		numberOfLines = Melder_iround (_numberOfLines->number);
	}
	UiPause_naturalvector (label->getString(), format, defaultString, numberOfLines);
	pushNumber (1);
}
static void do_boolean () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “boolean” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “boolean” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	double defaultNumber = undefined;
	if (defaultValue->which == Stackel_STRING) {
		conststring32 defaultString = defaultValue->getString();
		if (
			str32equ (defaultString, U"on") || str32equ (defaultString, U"yes") ||
			str32equ (defaultString, U"ON") || str32equ (defaultString, U"YES") ||
			str32equ (defaultString, U"On") || str32equ (defaultString, U"Yes")
		)
			defaultNumber = 1;
		else if (
			str32equ (defaultString, U"off") || str32equ (defaultString, U"no") ||
			str32equ (defaultString, U"OFF") || str32equ (defaultString, U"NO") ||
			str32equ (defaultString, U"Off") || str32equ (defaultString, U"No")
		)
			defaultNumber = 0;
		else
			Melder_throw (U"If the second argument of “boolean” is a string, it can only be "
					"“yes”, “no”, “on”, or “off”, not “", defaultString, U"”.");
	} else if (defaultValue->which == Stackel_NUMBER) {
		defaultNumber = defaultValue->number;
	} else {
		Melder_throw (U"The second argument of “boolean” (the default value) should be a number (0 or 1) "
				"or a string (“yes”, “no”, “on”, “off”), not ", defaultValue->whichText(), U".");
	}
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “boolean” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_boolean (label->getString(), defaultNumber != 0.0);
	pushNumber (1);
}
static void do_choice () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “choice” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “choice” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_NUMBER,
		U"The second argument of “choice” (the default value) should be a whole number, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “choice” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_choice (label->getString(), Melder_iround (defaultValue->number));
	pushNumber (1);
}
static void do_optionmenu () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “optionmenu” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “optionmenu” requires 2 arguments (a label text and a default value), not ", n->number, U".");
	const Stackel defaultValue = pop;
	Melder_require (defaultValue->which == Stackel_NUMBER,
		U"The second argument of “optionmenu” (the default value) should be a whole number, not ", defaultValue->whichText(), U".");
	const Stackel label = pop;
	Melder_require (label->which == Stackel_STRING,
		U"The first argument of “optionmenu” (the label text) should be a string, not ", label->whichText(), U".");
	UiPause_optionmenu (label->getString(), Melder_iround (defaultValue->number));
	pushNumber (1);
}
static void do_option () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “option” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “option” requires 1 argument (a text), not ", n->number, U".");
		const Stackel text = pop;
	Melder_require (text->which == Stackel_STRING,
		U"The argument of “option” should be a string (the text), not ", text->whichText(), U".");
	UiPause_option (text->getString());
	pushNumber (1);
}
static void do_heading () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “heading” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “heading” requires 1 argument (a text), not ", n->number, U".");
	const Stackel text = pop;
	Melder_require (text->which == Stackel_STRING,
		U"The argument of “heading” should be a string (the text), not ", text->whichText(), U".");
	UiPause_heading (text->getString());
	pushNumber (1);
}
static void do_comment () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “comment” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “comment” requires 1 argument (a text), not ", n->number, U".");
	const Stackel text = pop;
	Melder_require (text->which == Stackel_STRING,
		U"The argument of “comment” should be a string (the text), not ", text->whichText(), U".");
	UiPause_comment (text->getString());
	pushNumber (1);
}
static void do_endPause () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “endPause” is not available inside manuals.");
	const Stackel n = pop;
	if (n->number < 2 || n->number > 12)
		Melder_throw (U"The function “endPause” requires 2 to 12 arguments, not ", n->number, U".");
	const Stackel d = pop;
	if (d->which != Stackel_NUMBER)
		Melder_throw (U"The last argument of “endPause” should be a number (the default or cancel continue button), not ", d->whichText(), U".");
	/* mutable */ integer numberOfContinueButtons = Melder_iround (n->number) - 1;
	/* mutable */ integer cancelContinueButton = 0, defaultContinueButton = Melder_iround (d->number);
	Stackel ca = pop;
	if (ca->which == Stackel_NUMBER) {
		cancelContinueButton = defaultContinueButton;
		defaultContinueButton = Melder_iround (ca->number);
		numberOfContinueButtons -= 1;
		if (cancelContinueButton < 0 || cancelContinueButton > numberOfContinueButtons)
			Melder_throw (U"Your last argument of “endPause” is the number of the cancel button; it cannot be ", cancelContinueButton,
				U" but should be 0 (no cancel or stop button) or lie between 1 and ", numberOfContinueButtons, U".");
		if (cancelContinueButton == 0)
			cancelContinueButton = -1;
	}
	Stackel co [1+10] = { 0 };
	for (integer i = numberOfContinueButtons; i >= 1; i --) {
		co [i] = ( cancelContinueButton != 0 || i != numberOfContinueButtons ? pop : ca );
		if (co[i]->which != Stackel_STRING)
			Melder_throw (U"Each of the first ", numberOfContinueButtons,
				U" argument(s) of “endPause” should be a string (a button text), not ", co[i]->whichText(), U".");
	}
	const integer buttonClicked = UiPause_end (numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
		! co [1] ? nullptr : co[1]->getString(), ! co [2] ? nullptr : co[2]->getString(),
		! co [3] ? nullptr : co[3]->getString(), ! co [4] ? nullptr : co[4]->getString(),
		! co [5] ? nullptr : co[5]->getString(), ! co [6] ? nullptr : co[6]->getString(),
		! co [7] ? nullptr : co[7]->getString(), ! co [8] ? nullptr : co[8]->getString(),
		! co [9] ? nullptr : co[9]->getString(), ! co [10] ? nullptr : co[10]->getString(),
		theInterpreter);
	//Melder_casual (U"Button ", buttonClicked);
	pushNumber (buttonClicked);
}
static void do_lowerCaseAppNameStr () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “lowerCaseAppName$” requires 0 arguments, not ", n->number, U".");
	return pushString (Melder_dup (Melder_lowerCaseAppName()));
}
static void do_upperCaseAppNameStr () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “upperCaseAppName$” requires 0 arguments, not ", n->number, U".");
	return pushString (Melder_dup (Melder_upperCaseAppName()));
}
static void do_clock () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “clock” requires 0 arguments, not ", n->number, U".");
	return pushNumber (Melder_clock ());
}
static void do_appVersion () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appVersion” requires 0 arguments, not ", n->number, U".");
	return pushNumber (Melder_appVersion());
}
static void do_appVersionStr () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appVersion$” requires 0 arguments, not ", n->number, U".");
	return pushString (Melder_dup (Melder_appVersionSTR()));
}
static void do_appYear () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appYear” requires 0 arguments, not ", n->number, U".");
	return pushNumber (Melder_appYear());
}
static void do_appMonth () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appMonth” requires 0 arguments, not ", n->number, U".");
	return pushNumber (Melder_appMonth());
}
static void do_appMonthStr () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appMonth$” requires 0 arguments, not ", n->number, U".");
	return pushString (Melder_dup (Melder_appMonthSTR()));
}
static void do_appDay () {
	const Stackel n = pop;
	Melder_require (n->number == 0,
		U"The function “appDay” requires 0 arguments, not ", n->number, U".");
	return pushNumber (Melder_appDay());
}
static void do_chooseReadFileStr () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “chooseReadFile$” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “chooseReadFile$” requires 1 argument (a title), not ", n->number, U".");
	const Stackel title = pop;
	Melder_require (title->which == Stackel_STRING,
		U"The argument of “chooseReadFile$” should be a string (the title), not ", title->whichText(), U".");
	autoStringSet fileNames = GuiFileSelect_getInfileNames (nullptr, title->getString(), false);
	if (fileNames->size == 0)
		return pushString (Melder_dup (U""));
	SimpleString fileName = fileNames->at [1];
	pushString (Melder_dup (fileName -> string.get()));
}
static void do_chooseWriteFileStr () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “chooseWriteFile$” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 2,
		U"The function “chooseWriteFile$” requires 2 arguments (a title and a default name), not ", n->number, U".");
	const Stackel defaultName = pop, title = pop;
	Melder_require (title->which == Stackel_STRING && defaultName->which == Stackel_STRING,
		U"The arguments of “chooseWriteFile$” should be two strings (the title and the default name).");
	autostring32 result = GuiFileSelect_getOutfileName (nullptr, title->getString(), defaultName->getString());
	if (! result)
		result = Melder_dup (U"");
	pushString (result.move());
}
static void do_chooseFolder_STR () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “chooseFolder$” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “chooseFolder$” requires 1 argument (a title), not ", n->number, U".");
	const Stackel title = pop;
	Melder_require (title->which == Stackel_STRING,
		U"The argument of “chooseFolder$” should be a string (the title).");
	autostring32 result = GuiFileSelect_getFolderName (nullptr, title->getString());
	if (! result)
		result = Melder_dup (U"");
	pushString (result.move());
}
static void do_chooseDirectory_STR () {
	Melder_require (praat_commandsWithExternalSideEffectsAreAllowed (),
		U"The function “chooseDirectory$” is not available inside manuals.");
	const Stackel n = pop;
	Melder_require (n->number == 1,
		U"The function “chooseDirectory$” requires 1 argument (a title), not ", n->number, U".");
	const Stackel title = pop;
	Melder_require (title->which == Stackel_STRING,
		U"The argument of “chooseDirectory$” should be a string (the title).");
	autostring32 result = GuiFileSelect_getFolderName (nullptr, title->getString());
	if (! result)
		result = Melder_dup (U"");
	pushString (result.move());
}
static void do_demoWindowTitle () {
	const Stackel n = pop;
	if (n->number == 1) {
		const Stackel title = pop;
		if (title->which == Stackel_STRING) {
			Demo_windowTitle (title->getString());
		} else {
			Melder_throw (U"The argument of “demoWindowTitle” should be a string (the title), not ", title->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “demoWindowTitle” requires 1 argument (a title), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_demoShow () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoShow” requires 0 arguments, not ", n->number, U".");
	Demo_show ();
	pushNumber (1);
}
static void do_demoWaitForInput () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoWaitForInput” requires 0 arguments, not ", n->number, U".");
	Demo_waitForInput (theInterpreter);
	pushNumber (1);
}
static void do_demoPeekInput () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoPeekInput” requires 0 arguments, not ", n->number, U".");
	Demo_peekInput (theInterpreter);
	pushNumber (1);
}
static void do_demoInput () {
	const Stackel n = pop;
	if (n->number == 1) {
		const Stackel keys = pop;
		if (keys->which == Stackel_STRING) {
			const bool result = Demo_input (keys->getString());
			pushNumber (result);
		} else {
			Melder_throw (U"The argument of “demoInput” should be a string (the keys), not ", keys->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function “demoInput” requires 1 argument (keys), not ", n->number, U".");
	}
}
static void do_demoClickedIn () {
	const Stackel n = pop;
	if (n->number == 4) {
		const Stackel top = pop, bottom = pop, right = pop, left = pop;
		if (left->which == Stackel_NUMBER && right->which == Stackel_NUMBER && bottom->which == Stackel_NUMBER && top->which == Stackel_NUMBER) {
			const bool result = Demo_clickedIn (left->number, right->number, bottom->number, top->number);
			pushNumber (result);
		} else {
			Melder_throw (U"All arguments of “demoClickedIn” should be numbers (the x and y ranges).");
		}
	} else {
		Melder_throw (U"The function “demoClickedIn” requires 4 arguments (x and y ranges), not ", n->number, U".");
	}
}
static void do_demoClicked () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoClicked” requires 0 arguments, not ", n->number, U".");
	const bool result = Demo_clicked ();
	pushNumber (result);
}
static void do_demoX () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoX” requires 0 arguments, not ", n->number, U".");
	const double result = Demo_x ();
	pushNumber (result);
}
static void do_demoY () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoY” requires 0 arguments, not ", n->number, U".");
	const double result = Demo_y ();
	pushNumber (result);
}
static void do_demoKeyPressed () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoKeyPressed” requires 0 arguments, not ", n->number, U".");
	const bool result = Demo_keyPressed ();
	pushNumber (result);
}
static void do_demoKey () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoKey” requires 0 arguments, not ", n->number, U".");
	autostring32 key (1);
	key [0] = Demo_key ();
	pushString (key.move());
}
static void do_demoShiftKeyPressed () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoShiftKeyPressed” requires 0 arguments, not ", n->number, U".");
	const bool result = Demo_shiftKeyPressed ();
	pushNumber (result);
}
static void do_demoCommandKeyPressed () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoCommandKeyPressed” requires 0 arguments, not ", n->number, U".");
	const bool result = Demo_commandKeyPressed ();
	pushNumber (result);
}
static void do_demoOptionKeyPressed () {
	const Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function “demoOptionKeyPressed” requires 0 arguments, not ", n->number, U".");
	const bool result = Demo_optionKeyPressed ();
	pushNumber (result);
}
static integer Stackel_getRowNumber (Stackel row, Daata thee) {
	integer result = 0;
	if (row->which == Stackel_NUMBER) {
		result = Melder_iround (row->number);
	} else if (row->which == Stackel_STRING) {
		if (! thy v_hasGetRowIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have row labels, so row indexes have to be numbers.");
		result = Melder_iround (thy v_getRowIndex (row->getString()));
		if (result == 0)
			Melder_throw (U"Object “", thy name.get(), U"” has no row labelled “", row->getString(), U"”.");
	} else {
		Melder_throw (U"A row index should be a number or a string, not ", row->whichText(), U".");
	}
	return result;
}
static integer Stackel_getColumnNumber (Stackel column, Daata thee) {
	integer result = 0;
	if (column->which == Stackel_NUMBER) {
		result = Melder_iround (column->number);
	} else if (column->which == Stackel_STRING) {
		if (! thy v_hasGetColIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have column labels, so column indexes have to be numbers.");
		result = Melder_iround (thy v_getColIndex (column->getString()));
		if (result == 0)
			Melder_throw (U"Object ", thee, U" has no column labelled “", column->getString(), U"”.");
	} else {
		Melder_throw (U"A column index should be a number or a string, not ", column->whichText(), U".");
	}
	return result;
}
static void do_self0 (integer irow, integer icol) {
	const Daata me = theSource;
	if (! me)
		Melder_throw (U"The name “self” is restricted to formulas for objects.");
	if (my v_hasGetCell ()) {
		pushNumber (my v_getCell ());
	} else if (my v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop, hence no implicit column index for the current ",
				Thing_className (me), U" object (self).\nTry using the [column] index explicitly.");
		} else {
			pushNumber (my v_getVector (irow, icol));
		}
	} else if (my v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushNumber (my v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [] indexing.");
	}
}
static void do_selfStr0 (integer irow, integer icol) {
	const Daata me = theSource;
	if (! me)
		Melder_throw (U"The name “self$” is restricted to formulas for objects.");
	if (my v_hasGetCellStr ()) {
		pushString (Melder_dup (my v_getCellStr ()));
	} else if (my v_hasGetVectorStr ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop, hence no implicit column index for the current ",
				Thing_className (me), U" object (self).\nTry using the [column] index explicitly.");
		} else {
			pushString (Melder_dup (my v_getVectorStr (icol)));
		}
	} else if (my v_hasGetMatrixStr ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [] indexing.");
	}
}
static void do_toObject () {
	const Stackel object = pop;
	Daata thee = nullptr;
	if (object->which == Stackel_NUMBER) {
		integer i = theCurrentPraatObjects -> n;
		while (i > 0 && object->number != theCurrentPraatObjects -> list [i]. id)
			i --;
		if (i == 0)
			Melder_throw (U"No such object: ", object->number);
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else if (object->which == Stackel_STRING) {
		integer i = theCurrentPraatObjects -> n;
		while (i > 0 && ! Melder_equ (object->getString(), theCurrentPraatObjects -> list [i]. name.get()))
			i --;
		if (i == 0)
			Melder_throw (U"No such object: ", object->getString());
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else {
		Melder_throw (U"The first argument to “object” should be a number (unique ID) or a string (name), not ", object->whichText(), U".");
	}
	pushObject (thee);
}
static void do_objectCell0 (integer irow, integer icol) {
	const Stackel object = pop;
	const Daata thee = object->object;
	if (thy v_hasGetCell ()) {
		pushNumber (thy v_getCell ());
	} else if (thy v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit column index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, column].");
		} else {
			pushNumber (thy v_getVector (irow, icol));
		}
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for this ", Thing_className (thee), U" object.\n"
					U"Try using: object [id, row, column].");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
					U"Try using: object [id, row].");
			}
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [] indexing.");
	}
}
static void do_matrix0 (integer irow, integer icol) {
	const Daata thee = parse [programPointer]. content.object;
	if (thy v_hasGetCell ()) {
		pushNumber (thy v_getCell ());
	} else if (thy v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit column index for this ", Thing_className (thee), U" object.\n"
				U"Try using the [column] index explicitly.");
		} else {
			pushNumber (thy v_getVector (irow, icol));
		}
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for this ", Thing_className (thee), U" object.\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [] indexing.");
	}
}
static void do_selfMatrix1 (integer irow) {
	const Daata me = theSource;
	const Stackel column = pop;
	if (! me)
		Melder_throw (U"The name “self” is restricted to formulas for objects.");
	integer icol = Stackel_getColumnNumber (column, me);
	if (my v_hasGetVector ()) {
		pushNumber (my v_getVector (irow, icol));
	} else if (my v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for the current ", Thing_className (me), U" object (self).\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushNumber (my v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [column] indexes.");
	}
}
static void do_selfMatrix1_STR (integer irow) {
	const Daata me = theSource;
	const Stackel column = pop;
	if (! me)
		Melder_throw (U"The name “self$” is restricted to formulas for objects.");
	const integer icol = Stackel_getColumnNumber (column, me);
	if (my v_hasGetVectorStr ()) {
		pushString (Melder_dup (my v_getVectorStr (icol)));
	} else if (my v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for the current ", Thing_className (me), U" object (self).\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [column] indexes.");
	}
}
static void do_objectCell1 (integer irow) {
	const Stackel column = pop, object = pop;
	const Daata thee = object->object;
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVector ()) {
		pushNumber (thy v_getVector (irow, icol));
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, row, column].");
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes.");
	}
}
static void do_matrix1 (integer irow) {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel column = pop;
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVector ()) {
		pushNumber (thy v_getVector (irow, icol));
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes.");
	}
}
static void do_objectCell1_STR (integer irow) {
	const Stackel column = pop, object = pop;
	const Daata thee = object->object;
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		pushString (Melder_dup (thy v_getVectorStr (icol)));
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, row, column].");
		} else {
			pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_matrix1_STR (integer irow) {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel column = pop;
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		pushString (Melder_dup (thy v_getVectorStr (icol)));
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_selfMatrix2 () {
	const Daata me = theSource;
	Stackel column = pop, row = pop;
	if (! me)
		Melder_throw (U"The name “self” is restricted to formulas for objects.");
	const integer irow = Stackel_getRowNumber (row, me);
	const integer icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrix ())
		Melder_throw (Thing_className (me), U" objects like “self” accept no [row, column] indexing.");
	pushNumber (my v_getMatrix (irow, icol));
}
static void do_selfMatrix2_STR () {
	const Daata me = theSource;
	const Stackel column = pop, row = pop;
	if (! me)
		Melder_throw (U"The name “self$” is restricted to formulas for objects.");
	const integer irow = Stackel_getRowNumber (row, me);
	const integer icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrixStr ())
		Melder_throw (Thing_className (me), U" objects like “self$” accept no [row, column] indexing for string cells.");
	pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
}
static void do_objectCell2 () {
	const Stackel column = pop, row = pop, object = pop;
	const Daata thee = object->object;
	const integer irow = Stackel_getRowNumber (row, thee);
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_matrix2 () {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel column = pop, row = pop;
	const integer irow = Stackel_getRowNumber (row, thee);
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_objectCell2_STR () {
	const Stackel column = pop, row = pop, object = pop;
	const Daata thee = object->object;
	const integer irow = Stackel_getRowNumber (row, thee);
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing for string cells.");
	pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
}
static void do_matrix2_STR () {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel column = pop, row = pop;
	const integer irow = Stackel_getRowNumber (row, thee);
	const integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing for string cells.");
	pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
}
static void do_objectLocation0 (integer irow, integer icol) {
	const Stackel object = pop;
	const Daata thee = object->object;
	if (thy v_hasGetFunction0 ()) {
		pushNumber (thy v_getFunction0 ());
	} else if (thy v_hasGetFunction1 ()) {
		const Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x).");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me),
				U" object gives no implicit x values,\nhence no implicit x value for this ",
				Thing_className (thee), " object.\n"
				U"Try using: object (id, x).");
		const double x = my v_getX (icol);
		pushNumber (thy v_getFunction1 (irow, x));
	} else if (thy v_hasGetFunction2 ()) {
		const Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x or y values for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x, y).");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit x values,\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x, y).");
		const double x = my v_getX (icol);
		if (! my v_hasGetY ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, y).");
		const double y = my v_getY (irow);
		pushNumber (thy v_getFunction2 (x, y));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no () values.");
	}
}
static void do_function0 (integer irow, integer icol) {
	const Daata thee = parse [programPointer]. content.object;
	if (thy v_hasGetFunction0 ()) {
		pushNumber (thy v_getFunction0 ());
	} else if (thy v_hasGetFunction1 ()) {
		const Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using the (x) argument explicitly.");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me),
				U" object gives no implicit x values,\nhence no implicit x value for this ",
				Thing_className (thee), U" object.\n"
				U"Try using the (x) argument explicitly.");
		const double x = my v_getX (icol);
		pushNumber (thy v_getFunction1 (irow, x));
	} else if (thy v_hasGetFunction2 ()) {
		const Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x or y values for this ", Thing_className (thee), U" object.\n"
				U"Try using both (x, y) arguments explicitly.");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit x values,\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using both (x, y) arguments explicitly.");
		const double x = my v_getX (icol);
		if (! my v_hasGetY ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using the (y) argument explicitly.");
		const double y = my v_getY (irow);
		pushNumber (thy v_getFunction2 (x, y));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no () values.");
	}
}
static void do_selfFunction1 (integer irow) {
	const Daata me = theSource;
	const Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (! me)
			Melder_throw (U"The name “self” is restricted to formulas for objects.");
		if (my v_hasGetFunction1 ()) {
			pushNumber (my v_getFunction1 (irow, x->number));
		} else if (my v_hasGetFunction2 ()) {
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object (self) accepts no implicit y values.\n"
					U"Try using both (x, y) arguments instead.");
			const double y = my v_getY (irow);
			pushNumber (my v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (me), U" objects like “self” accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (me), U" objects like “self” accept only numeric x values.");
	}
}
static void do_objectLocation1 (integer irow) {
	const Stackel x = pop, object = pop;
	const Daata thee = object->object;
	if (x->which == Stackel_NUMBER) {
		if (thy v_hasGetFunction1 ()) {
			pushNumber (thy v_getFunction1 (irow, x->number));
		} else if (thy v_hasGetFunction2 ()) {
			const Daata me = theSource;
			if (! me)
				Melder_throw (U"No current object (we are not in a Formula command),\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, x, y).");
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, x, y).");
			const double y = my v_getY (irow);
			pushNumber (thy v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (thee), U" objects accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_function1 (integer irow) {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (thy v_hasGetFunction1 ()) {
			pushNumber (thy v_getFunction1 (irow, x->number));
		} else if (thy v_hasGetFunction2 ()) {
			const Daata me = theSource;
			if (! me)
				Melder_throw (U"No current object (we are not in a Formula command),\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using both (x, y) arguments instead.");
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using both (x, y) arguments instead.");
			const double y = my v_getY (irow);
			pushNumber (thy v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (thee), U" objects accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_selfFunction2 () {
	const Daata me = theSource;
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! me)
			Melder_throw (U"The name “self” is restricted to formulas for objects.");
		if (! my v_hasGetFunction2 ())
			Melder_throw (Thing_className (me), U" objects like “self” accept no (x, y) values.");
		pushNumber (my v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (me), U" objects accept only numeric x values.");
	}
}
static void do_objectLocation2 () {
	const Stackel y = pop, x = pop, object = pop;
	const Daata thee = object->object;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! thy v_hasGetFunction2 ())
			Melder_throw (Thing_className (thee), U" objects accept no (x, y) values.");
		pushNumber (thy v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_function2 () {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! thy v_hasGetFunction2 ())
			Melder_throw (Thing_className (thee), U" objects accept no (x, y) values.");
		pushNumber (thy v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_row_STR () {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel row = pop;
	const integer irow = Stackel_getRowNumber (row, thee);
	autostring32 result = Melder_dup (thy v_getRowStr (irow));
	if (! result)
		Melder_throw (U"Row index out of bounds.");
	pushString (result.move());
}
static void do_col_STR () {
	const Daata thee = parse [programPointer]. content.object;
	const Stackel col = pop;
	const integer icol = Stackel_getColumnNumber (col, thee);
	autostring32 result = Melder_dup (thy v_getColStr (icol));
	if (! result)
		Melder_throw (U"Column index out of bounds.");
	pushString (result.move());
}

static double NUMerf (double x) {
	return 1.0 - NUMerfcc (x);
}

void Formula_run (integer row, integer col, Formula_Result *result) {
	FormulaInstruction f = parse;
	programPointer = 1;   // first symbol of the program
	if (! theStack) {
		theStack = Melder_calloc_f (structStackel, 1+Formula_MAXIMUM_STACK_SIZE);
		if (! theStack)
			Melder_throw (U"Out of memory during formula computation.");
	}
	stackPointer = 0;   // start new stack
	stackPointerMax = 0;   // start new stack
	try {
		while (programPointer <= numberOfInstructions) {
			integer symbol;
				switch (symbol = f [programPointer]. symbol) {

case NUMBER_: { pushNumber (f [programPointer]. content.number);
} break; case STOPWATCH_: { pushNumber (Melder_stopwatch ());
} break; case ROW_: { pushNumber (row);
} break; case COL_: { pushNumber (col);
} break; case X_: {
	Daata me = theSource;
	Melder_require (my v_hasGetX (),
		U"No values for “x” for this object.");
	pushNumber (my v_getX (col));
} break; case Y_: {
	Daata me = theSource;
	Melder_require (my v_hasGetY (),
		U"No values for “y” for this object.");
	pushNumber (my v_getY (row));
} break; case NOT_: { do_not ();
} break; case EQ_: { do_eq ();
} break; case NE_: { do_ne ();
} break; case LE_: { do_le ();
} break; case LT_: { do_lt ();
} break; case GE_: { do_ge ();
} break; case GT_: { do_gt ();
} break; case ADD_: { do_add ();
} break; case SUB_: { do_sub ();
} break; case MUL_: { do_mul ();
} break; case RDIV_: { do_rdiv ();
} break; case IDIV_: { do_idiv ();
} break; case MOD_: { do_mod ();
} break; case MINUS_: { do_minus ();
} break; case POWER_: { do_power ();
/********** Functions of 1 variable: **********/

#define CASE_NUM_WITH_TENSORS(label, function)  \
} break; case label: { function (); \
} break; case label##VEC_: { function##_VEC (); \
} break; case label##MAT_: { function##_MAT ();
CASE_NUM_WITH_TENSORS (ABS_, do_abs)
CASE_NUM_WITH_TENSORS (ROUND_, do_round)
CASE_NUM_WITH_TENSORS (FLOOR_, do_floor)
CASE_NUM_WITH_TENSORS (CEILING_, do_ceiling)
CASE_NUM_WITH_TENSORS (RECTIFY_, do_rectify)
CASE_NUM_WITH_TENSORS (SQRT_, do_sqrt)
CASE_NUM_WITH_TENSORS (SIN_, do_sin)
CASE_NUM_WITH_TENSORS (COS_, do_cos)
CASE_NUM_WITH_TENSORS (TAN_, do_tan)
CASE_NUM_WITH_TENSORS (ARCSIN_, do_arcsin)
CASE_NUM_WITH_TENSORS (ARCCOS_, do_arccos)
CASE_NUM_WITH_TENSORS (ARCTAN_, do_arctan)
} break; case SINC_: { do_function_n_n (NUMsinc);
} break; case SINCPI_: { do_function_n_n (NUMsincpi);
CASE_NUM_WITH_TENSORS (EXP_, do_exp)
CASE_NUM_WITH_TENSORS (SINH_, do_sinh)
CASE_NUM_WITH_TENSORS (COSH_, do_cosh)
CASE_NUM_WITH_TENSORS (TANH_, do_tanh)
CASE_NUM_WITH_TENSORS (ARCSINH_, do_arcsinh)
CASE_NUM_WITH_TENSORS (ARCCOSH_, do_arccosh)
CASE_NUM_WITH_TENSORS (ARCTANH_, do_arctanh)
CASE_NUM_WITH_TENSORS (SIGMOID_, do_sigmoid)
CASE_NUM_WITH_TENSORS (INV_SIGMOID_, do_invSigmoid)
} break; case SOFTMAX_VEC_: { do_softmax_VEC ();
} break; case SOFTMAX_PER_ROW_MAT_: { do_softmaxPerRow_MAT ();
} break; case ERF_: { do_function_n_n (NUMerf);
} break; case ERFC_: { do_function_n_n (NUMerfcc);
} break; case GAUSS_P_: { do_function_n_n (NUMgaussP);
} break; case GAUSS_Q_: { do_function_n_n (NUMgaussQ);
} break; case INV_GAUSS_Q_: { do_function_n_n (NUMinvGaussQ);
} break; case RANDOM_BERNOULLI_: { do_function_n_n (NUMrandomBernoulli_real);
} break; case RANDOM_BERNOULLI_VEC_: { do_functionvec_n_n (NUMrandomBernoulli_real);
} break; case RANDOM_POISSON_: { do_function_n_n (NUMrandomPoisson);
} break; case TRANSPOSE_MAT_: { do_transpose_MAT ();
} break; case ROW_SUMS_VEC_: { do_rowSums_VEC ();
} break; case COLUMN_SUMS_VEC_: { do_columnSums_VEC ();
CASE_NUM_WITH_TENSORS (LOG2_, do_log2)
CASE_NUM_WITH_TENSORS (LN_, do_ln)
CASE_NUM_WITH_TENSORS (LOG10_, do_log10)
} break; case LN_GAMMA_: { do_function_n_n (NUMlnGamma);
} break; case HERTZ_TO_BARK_: { do_function_n_n (NUMhertzToBark);
} break; case BARK_TO_HERTZ_: { do_function_n_n (NUMbarkToHertz);
} break; case PHON_TO_DIFFERENCE_LIMENS_: { do_function_n_n (NUMphonToDifferenceLimens);
} break; case DIFFERENCE_LIMENS_TO_PHON_: { do_function_n_n (NUMdifferenceLimensToPhon);
} break; case HERTZ_TO_MEL_: { do_function_n_n (NUMhertzToMel);
} break; case MEL_TO_HERTZ_: { do_function_n_n (NUMmelToHertz);
} break; case HERTZ_TO_SEMITONES_: { do_function_n_n (NUMhertzToSemitones);
} break; case SEMITONES_TO_HERTZ_: { do_function_n_n (NUMsemitonesToHertz);
} break; case ERB_: { do_function_n_n (NUMerb);
} break; case HERTZ_TO_ERB_: { do_function_n_n (NUMhertzToErb);
} break; case ERB_TO_HERTZ_: { do_function_n_n (NUMerbToHertz);
} break; case SUM_: { do_sum ();
} break; case MEAN_: { do_mean ();
} break; case STDEV_: { do_stdev ();
} break; case CENTER_: { do_center ();
} break; case RANDOM_IMAX_: { do_randomImax ();
} break; case EVALUATE_: { do_evaluate ();
} break; case EVALUATE_NOCHECK_: { do_evaluate_nocheck ();
} break; case EVALUATE_STR_: { do_evaluate_STR ();
} break; case EVALUATE_NOCHECK_STR_: { do_evaluate_nocheck_STR ();
/********** Functions of 2 numerical variables: **********/
} break; case ARCTAN2_: { do_function_dd_d (atan2);
} break; case RANDOM_UNIFORM_: { do_function_dd_d (NUMrandomUniform);
} break; case RANDOM_INTEGER_: { do_function_ll_l (NUMrandomInteger);
} break; case RANDOM_GAUSS_: { do_function_dd_d (NUMrandomGauss);
} break; case RANDOM_BINOMIAL_: { do_function_dl_d (NUMrandomBinomial_real);
} break; case RANDOM_GAMMA_: { do_function_dd_d (NUMrandomGamma);
} break; case CHI_SQUARE_P_: { do_function_dd_d (NUMchiSquareP);
} break; case CHI_SQUARE_Q_: { do_function_dd_d (NUMchiSquareQ);
} break; case INCOMPLETE_GAMMAP_: { do_function_dd_d (NUMincompleteGammaP);
} break; case INV_CHI_SQUARE_Q_: { do_function_dd_d (NUMinvChiSquareQ);
} break; case STUDENT_P_: { do_function_dd_d (NUMstudentP);
} break; case STUDENT_Q_: { do_function_dd_d (NUMstudentQ);
} break; case INV_STUDENT_Q_: { do_function_dd_d (NUMinvStudentQ);
} break; case BETA_: { do_function_dd_d (NUMbeta);
} break; case BETA2_: { do_function_dd_d (NUMbeta2);
} break; case BESSEL_I_: { do_function_ld_d (NUMbesselI);
} break; case BESSEL_K_: { do_function_ld_d (NUMbesselK);
} break; case LN_BETA_: { do_function_dd_d (NUMlnBeta);
} break; case SOUND_PRESSURE_TO_PHON_: { do_function_dd_d (NUMsoundPressureToPhon);
} break; case OBJECTS_ARE_IDENTICAL_: { do_objects_are_identical ();
/********** Functions of 3 numerical variables: **********/
} break; case FISHER_P_: { do_function_ddd_d (NUMfisherP);
} break; case FISHER_Q_: { do_function_ddd_d (NUMfisherQ);
} break; case INV_FISHER_Q_: { do_function_ddd_d (NUMinvFisherQ);
} break; case BINOMIAL_P_: { do_function_ddd_d (NUMbinomialP);
} break; case BINOMIAL_Q_: { do_function_ddd_d (NUMbinomialQ);
} break; case INCOMPLETE_BETA_: { do_function_ddd_d (NUMincompleteBeta);
} break; case INV_BINOMIAL_P_: { do_function_ddd_d (NUMinvBinomialP);
} break; case INV_BINOMIAL_Q_: { do_function_ddd_d (NUMinvBinomialQ);
/********** Functions of a variable number of variables: **********/
} break; case DO_   : { do_do    ();
} break; case DOSTR_: { do_do_STR ();
} break; case WRITE_INFO_      : { do_writeInfo      ();
} break; case WRITE_INFO_LINE_ : { do_writeInfoLine  ();
} break; case APPEND_INFO_     : { do_appendInfo     ();
} break; case APPEND_INFO_LINE_: { do_appendInfoLine ();
} break; case WRITE_FILE_      : { do_writeFile      ();
} break; case WRITE_FILE_LINE_ : { do_writeFileLine  ();
} break; case APPEND_FILE_     : { do_appendFile     ();
} break; case APPEND_FILE_LINE_: { do_appendFileLine ();
} break; case PAUSE_SCRIPT_: { do_pauseScript ();
} break; case EXIT_SCRIPT_: { do_exitScript ();
} break; case RUN_SCRIPT_: { do_runScript ();
} break; case RUN_SCRIPT_WITH_FORM_: { do_runScriptWithForm ();
} break; case RUN_NOTEBOOK_: { do_runNotebook ();
} break; case RUN_SYSTEM_: { do_runSystem ();
} break; case RUN_SYSTEM_STR_: { do_runSystem_STR ();
} break; case RUN_SYSTEM_NOCHECK_: { do_runSystem_nocheck ();
} break; case RUN_SUBPROCESS_: { do_runSubprocess ();
} break; case RUN_SUBPROCESS_STR_: { do_runSubprocess_STR ();
} break; case MIN_: { do_min ();
} break; case MIN_E_: { do_min_e ();
} break; case MIN_IGNORE_UNDEFINED_: { do_min_removeUndefined ();
} break; case MAX_: { do_max ();
} break; case MAX_E_: { do_max_e ();
} break; case MAX_IGNORE_UNDEFINED_: { do_max_removeUndefined ();
} break; case IMIN_: { do_imin ();
} break; case IMIN_E_: { do_imin_e ();
} break; case IMIN_IGNORE_UNDEFINED_: { do_imin_removeUndefined ();
} break; case IMAX_: { do_imax ();
} break; case IMAX_E_: { do_imax_e ();
} break; case IMAX_IGNORE_UNDEFINED_: { do_imax_removeUndefined ();
} break; case QUANTILE_: { do_quantile ();
} break; case NORM_: { do_norm ();
} break; case ZERO_VEC_: { do_zero_VEC ();
} break; case ZERO_MAT_: { do_zero_MAT ();
} break; case LINEAR_VEC_: { do_linear_VEC ();
} break; case TO_VEC_: { do_to_VEC ();
} break; case FROM_TO_VEC_: { do_from_to_VEC ();
} break; case FROM_TO_BY_VEC_: { do_from_to_by_VEC ();
} break; case FROM_TO_COUNT_VEC_: { do_from_to_count_VEC ();
} break; case BETWEEN_BY_VEC_: { do_between_by_VEC ();
} break; case BETWEEN_COUNT_VEC_: { do_between_count_VEC ();
} break; case SORT_VEC_: { do_sort_VEC ();
} break; case SORT_REMOVE_UNDEFINED_VEC_: { do_sort_removeUndefined_VEC ();
} break; case SORT_STRVEC_: { do_sort_STRVEC ();
} break; case SORT_NUMBER_AWARE_STRVEC_: { do_sort_numberAware_STRVEC ();
} break; case SHUFFLE_VEC_: { do_shuffle_VEC ();
} break; case SHUFFLE_STRVEC_: { do_shuffle_STRVEC ();
} break; case RANDOM_UNIFORM_VEC_: { do_function_VECdd_d (NUMrandomUniform);
} break; case RANDOM_UNIFORM_MAT_: { do_function_MATdd_d (NUMrandomUniform);
} break; case RANDOM_INTEGER_VEC_: { do_function_VECll_l (NUMrandomInteger);
} break; case RANDOM_INTEGER_MAT_: { do_function_MATll_l (NUMrandomInteger);
} break; case RANDOM_GAUSS_VEC_: { do_function_VECdd_d (NUMrandomGauss);
} break; case RANDOM_GAUSS_MAT_: { do_function_MATdd_d (NUMrandomGauss);
} break; case RANDOM_GAMMA_VEC_: { do_function_VECdd_d (NUMrandomGamma);
} break; case RANDOM_GAMMA_MAT_: { do_function_MATdd_d (NUMrandomGamma);
} break; case SOLVE_SPARSE_VEC_ : { do_solveSparse_VEC ();
} break; case SOLVE_NONNEGATIVE_VEC_ : { do_solveNonnegative_VEC ();
} break; case PEAKS_MAT_: { do_peaks_MAT ();
} break; case SIZE_: { do_size ();
} break; case NUMBER_OF_ROWS_: { do_numberOfRows ();
} break; case NUMBER_OF_COLUMNS_: { do_numberOfColumns ();
} break; case COMBINE_VEC_: { do_combine_VEC ();
} break; case PART_VEC_: { do_part_VEC ();
} break; case PART_MAT_: { do_part_MAT ();
} break; case EDITOR_: { do_editor ();
} break; case RANDOM__INITIALIZE_WITH_SEED_UNSAFELY_BUT_PREDICTABLY_: { do_random_initializeWithSeedUnsafelyButPredictably ();
} break; case RANDOM__INITIALIZE_SAFELY_AND_UNPREDICTABLY_: { do_random_initializeSafelyAndUnpredictably ();
} break; case HASH_: { do_hash ();
} break; case HEX_STR_: { do_hex_STR ();
} break; case UNHEX_STR_: { do_unhex_STR ();
} break; case EMPTY_STRVEC_: { do_empty_STRVEC ();
} break; case READ_LINES_FROM_FILE_STRVEC_: { do_readLinesFromFile_STRVEC ();
} break; case FILE_NAMES_STRVEC_: { do_fileNames_STRVEC ();
} break; case FOLDER_NAMES_STRVEC_: { do_folderNames_STRVEC ();
} break; case FILE_NAMES_CASE_INSENSITIVE_STRVEC_: { do_fileNames_caseInsensitive_STRVEC ();
} break; case FOLDER_NAMES_CASE_INSENSITIVE_STRVEC_: { do_folderNames_caseInsensitive_STRVEC ();
} break; case SPLIT_BY_WHITESPACE_STRVEC_: { do_splitByWhitespace_STRVEC ();
} break; case SPLIT_BY_STRVEC_: { do_splitBy_STRVEC ();
} break; case LOWER_CASE_STR_: { do_lowerCase_STR ();
} break; case LOWER_CAMEL_CASE_STR_: { do_lowerCamelCase_STR ();
} break; case LOWER_SNAKE_CASE_STR_: { do_lowerSnakeCase_STR ();
} break; case UPPER_CASE_STR_: { do_upperCase_STR ();
} break; case UPPER_CAMEL_CASE_STR_: { do_upperCamelCase_STR ();
} break; case UPPER_SNAKE_CASE_STR_: { do_upperSnakeCase_STR ();
/********** String functions of 1 variable: **********/
} break; case LENGTH_: { do_length ();
} break; case STRING_TO_NUMBER_: { do_number ();
} break; case FILE_READABLE_: { do_fileReadable ();
} break; case FOLDER_EXISTS_: { do_folderExists ();
} break; case TRY_TO_WRITE_FILE_: { do_tryToWriteFile ();
} break; case TRY_TO_APPEND_FILE_: { do_tryToAppendFile ();
/********** String functions of 0 variables: **********/
} break; case DATE_STR_: { do_date_STR ();
} break; case DATE_UTC_STR_: { do_date_utc_STR ();
} break; case DATE_ISO_STR_: { do_date_iso_STR ();
} break; case DATE_UTC_ISO_STR_: { do_date_utc_iso_STR ();
} break; case DATE_VEC_: { do_date_VEC ();
} break; case DATE_UTC_VEC_: { do_date_utc_VEC ();
} break; case INFO_STR_: { do_info_STR ();
/********** String functions of 2 variables: **********/
} break; case LEFT_STR_: { do_left_STR ();
} break; case RIGHT_STR_: { do_right_STR ();
} break; case MID_STR_: { do_mid_STR ();
} break; case PAD_LEFT_STR_: { do_padLeft_STR ();
} break; case PAD_RIGHT_STR_: { do_padRight_STR ();
} break; case TRUNCATE_LEFT_STR_: { do_truncateLeft_STR ();
} break; case TRUNCATE_RIGHT_STR_: { do_truncateRight_STR ();
} break; case PAD_OR_TRUNCATE_LEFT_STR_: { do_padOrTruncateLeft_STR ();
} break; case PAD_OR_TRUNCATE_RIGHT_STR_: { do_padOrTruncateRight_STR ();
} break; case UNICODE_TO_BACKSLASH_TRIGRAPHS_STR_: { do_unicodeToBackslashTrigraphs_STR ();
} break; case BACKSLASH_TRIGRAPHS_TO_UNICODE_STR_: { do_backslashTrigraphsToUnicode_STR ();
} break; case ENVIRONMENT_STR_: { do_environment_STR ();
} break; case INDEX_: { do_index ();
} break; case INDEX_CASE_INSENSITIVE_: { do_index_caseInsensitive ();
} break; case RINDEX_: { do_rindex ();
} break; case RINDEX_CASE_INSENSITIVE_: { do_rindex_caseInsensitive ();
} break; case STARTS_WITH_: { do_stringMatchesCriterion (kMelder_string::STARTS_WITH, true);
} break; case STARTS_WITH_CASE_INSENSITIVE_: { do_stringMatchesCriterion (kMelder_string::STARTS_WITH, false);
} break; case ENDS_WITH_: { do_stringMatchesCriterion (kMelder_string::ENDS_WITH, true);
} break; case ENDS_WITH_CASE_INSENSITIVE_: { do_stringMatchesCriterion (kMelder_string::ENDS_WITH, false);
} break; case INDEX_REGEX_: { do_index_regex (false);
} break; case RINDEX_REGEX_: { do_index_regex (true);
} break; case EXTRACT_NUMBER_: { do_extractNumber ();
/********** Other string functions: **********/
} break; case EXTRACT_WORD_STR_: { do_extractText_STR (true);
} break; case EXTRACT_LINE_STR_: { do_extractText_STR (false);
} break; case REPLACE_STR_: { do_replace_STR ();
} break; case REPLACE_REGEX_STR_: { do_replace_regex_STR ();
} break; case SELECTED_: { do_selected ();
} break; case SELECTED_STR_: { do_selected_STR ();
} break; case NUMBER_OF_SELECTED_: { do_numberOfSelected ();
} break; case SELECTED_VEC_: { do_selected_VEC ();
} break; case SELECTED_STRVEC_: { do_selected_STRVEC ();
} break; case SELECT_OBJECT_: { do_selectObject ();
} break; case PLUS_OBJECT_  : { do_plusObject   ();
} break; case MINUS_OBJECT_ : { do_minusObject  ();
} break; case REMOVE_OBJECT_: { do_removeObject ();
} break; case OBJECT_XMIN_: { do_object_xmin ();
} break; case OBJECT_XMAX_: { do_object_xmax ();
} break; case OBJECT_YMIN_: { do_object_ymin ();
} break; case OBJECT_YMAX_: { do_object_ymax ();
} break; case OBJECT_NX_: { do_object_nx ();
} break; case OBJECT_NY_: { do_object_ny ();
} break; case OBJECT_DX_: { do_object_dx ();
} break; case OBJECT_DY_: { do_object_dy ();
} break; case OBJECT_NROW_: { do_object_nrow ();
} break; case OBJECT_NCOL_: { do_object_ncol ();
} break; case OBJECT_ROW_STR_: { do_object_row_STR ();
} break; case OBJECT_COL_STR_: { do_object_col_STR ();
} break; case STRING_STR_: { do_string_STR ();
} break; case VERTICAL_STR_: { do_vertical_STR ();
} break; case NUMBERS_VEC_: { do_numbers_VEC ();
} break; case SLEEP_: { do_sleep ();
} break; case UNICODE_: { do_unicode ();
} break; case UNICODE_STR_: { do_unicode_STR ();
} break; case FIXED_STR_: { do_fixed_STR ();
} break; case PERCENT_STR_: { do_percent_STR ();
} break; case HEXADECIMAL_STR_: { do_hexadecimal_STR ();
} break; case DELETE_FILE_: { do_deleteFile ();
} break; case MOVE_AND_OR_RENAME_FILE_: { do_moveAndOrRenameFile ();
} break; case CREATE_FOLDER_: { do_createFolder ();
} break; case CREATE_DIRECTORY_: { do_createDirectory ();   // deprecated 2020
} break; case SET_WORKING_DIRECTORY_: { do_setWorkingDirectory ();   // deprecated 2020
} break; case VARIABLE_EXISTS_: { do_variableExists ();
} break; case READ_FILE_: { do_readFile ();
} break; case READ_FILE_STR_: { do_readFile_STR ();
} break; case READ_FILE_VEC_: { do_readFile_VEC ();
} break; case READ_FILE_MAT_: { do_readFile_MAT ();
/********** Matrix functions: **********/
} break; case ROW_VEC_: { do_row_VEC ();
} break; case COL_VEC_: { do_col_VEC ();
} break; case INNER_: { do_inner ();
} break; case CORRELATION_: { do_correlation ();
} break; case OUTER_MAT_: { do_outer_MAT ();
} break; case MUL_VEC_: { do_mul_VEC ();
} break; case MUL_MAT_: { do_mul_MAT ();
} break; case MUL_FAST_MAT_: { do_mul_fast_MAT ();
} break; case MUL_METAL_MAT_: { do_mul_metal_MAT ();
} break; case MUL_TN_MAT_: { do_mul_tn_MAT ();
} break; case MUL_NT_MAT_: { do_mul_nt_MAT ();
} break; case MUL_TT_MAT_: { do_mul_tt_MAT ();
} break; case REPEAT_VEC_: { do_repeat_VEC ();
} break; case ROW_INNERS_VEC_: { do_rowInners_VEC ();
} break; case SOLVE_VEC_: { do_solve_VEC ();
} break; case SOLVE_MAT_: { do_solve_MAT ();
} break; case SOLVE_WEAKLYCONSTRAINED_VEC_: { do_solveWeaklyConstrained_VEC ();
/********** Pause window functions: **********/
} break; case ASK_FOR_TRUST_: { do_askForTrust ();
} break; case BEGIN_PAUSE_: { do_beginPause ();
} break; case REAL_: { do_real ();
} break; case POSITIVE_: { do_positive ();
} break; case INTEGER_: { do_integer ();
} break; case NATURAL_: { do_natural ();
} break; case WORD_: { do_word ();
} break; case SENTENCE_: { do_sentence ();
} break; case TEXT_: { do_text ();
} break; case BOOLEAN_: { do_boolean ();
} break; case CHOICE_: { do_choice ();
} break; case OPTIONMENU_: { do_optionmenu ();
} break; case OPTION_MENU_: { do_optionmenu ();   // alternative spelling, deprecated 2023
} break; case OPTION_: { do_option ();
} break; case INFILE_: { do_infile ();
} break; case OUTFILE_: { do_outfile ();
} break; case FOLDER_: { do_folder ();
} break; case REALVECTOR_: { do_realvector ();
} break; case POSITIVEVECTOR_: { do_positivevector ();
} break; case INTEGERVECTOR_: { do_integervector ();
} break; case NATURALVECTOR_: { do_naturalvector ();
} break; case HEADING_: { do_heading ();
} break; case COMMENT_: { do_comment ();
} break; case END_PAUSE_: { do_endPause ();
} break; case LOWER_CASE_APP_NAME_STR_: { do_lowerCaseAppNameStr ();
} break; case UPPER_CASE_APP_NAME_STR_: { do_upperCaseAppNameStr ();
} break; case CLOCK_: { do_clock ();
} break; case APP_VERSION_: { do_appVersion ();
} break; case APP_VERSION_STR_: { do_appVersionStr ();
} break; case APP_YEAR_: { do_appYear ();
} break; case APP_MONTH_: { do_appMonth ();
} break; case APP_MONTH_STR_: { do_appMonthStr ();
} break; case APP_DAY_: { do_appDay ();
} break; case CHOOSE_READ_FILE_STR_: { do_chooseReadFileStr ();
} break; case CHOOSE_WRITE_FILE_STR_: { do_chooseWriteFileStr ();
} break; case CHOOSE_FOLDER_STR_: { do_chooseFolder_STR ();
} break; case CHOOSE_DIRECTORY_STR_: { do_chooseDirectory_STR ();   // deprecated 2020
/********** Demo window functions: **********/
} break; case DEMO_WINDOW_TITLE_: { do_demoWindowTitle ();
} break; case DEMO_SHOW_: { do_demoShow ();
} break; case DEMO_WAIT_FOR_INPUT_: { do_demoWaitForInput ();
} break; case DEMO_PEEK_INPUT_: { do_demoPeekInput ();
} break; case DEMO_INPUT_: { do_demoInput ();
} break; case DEMO_CLICKED_IN_: { do_demoClickedIn ();
} break; case DEMO_CLICKED_: { do_demoClicked ();
} break; case DEMO_X_: { do_demoX ();
} break; case DEMO_Y_: { do_demoY ();
} break; case DEMO_KEY_PRESSED_: { do_demoKeyPressed ();
} break; case DEMO_KEY_: { do_demoKey ();
} break; case DEMO_SHIFT_KEY_PRESSED_: { do_demoShiftKeyPressed ();
} break; case DEMO_COMMAND_KEY_PRESSED_: { do_demoCommandKeyPressed ();
} break; case DEMO_OPTION_KEY_PRESSED_: { do_demoOptionKeyPressed ();
/********** **********/
} break; case TRUE_: {
	pushNumber (1.0);
} break; case FALSE_: {
	pushNumber (0.0);
} break; case IFTRUE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->number != 0.0) {
/* Possible compiler BUG: some compilers cannot handle the following assignment. */
/* Those compilers will have trouble with praat's AND and OR. */
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_throw (U"A condition between “if” and “then” should be a number, not ", condition->whichText(), U".");
	}
} break; case IFFALSE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->number == 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_throw (U"A condition between “if” and “then” should be a number, not ", condition->whichText(), U".");
	}
} break; case GOTO_: {
	programPointer = f [programPointer]. content.label - theOptimize;
} break; case LABEL_: {
	;
} break; case DECREMENT_AND_ASSIGN_: {
	Stackel x = pop, v = pop;
	InterpreterVariable var = v->variable;
	var -> numericValue = x->number - 1.0;
	//Melder_casual (U"starting value ", var -> numericValue);
	pushVariable (var);
} break; case INCREMENT_GREATER_GOTO_: {
	//Melder_casual (U"top of loop, stack depth ", stackPointer);
	Stackel e = & theStack [stackPointer], v = & theStack [stackPointer - 1];
	Melder_assert (e->which == Stackel_NUMBER);
	Melder_assert (v->which == Stackel_VARIABLE);
	InterpreterVariable var = v->variable;
	//Melder_casual (U"loop variable ", var -> numericValue);
	var -> numericValue += 1.0;
	//Melder_casual (U"loop variable ", var -> numericValue);
	//Melder_casual (U"end value ", e->number);
	if (var -> numericValue > e->number) {
		programPointer = f [programPointer]. content.label - theOptimize;
	}
} break; case ADD_3DOWN_: {
	Stackel x = pop, s = & theStack [stackPointer - 2];
	Melder_assert (x->which == Stackel_NUMBER);
	Melder_assert (s->which == Stackel_NUMBER);
	//Melder_casual (U"to add ", x->number);
	s->number += x->number;
	//Melder_casual (U"sum ", s->number);
} break; case POP_2_: {
	stackPointer -= 2;
	//Melder_casual (U"total ", theStack [stackPointer]. number);
} break; case VEC_CELL_: { do_numericVectorElement ();
} break; case MAT_CELL_: { do_numericMatrixElement ();
} break; case STRVEC_CELL_: { do_stringVectorElement ();
} break; case INDEXED_NUMERIC_VARIABLE_: { do_indexedNumericVariable ();
} break; case INDEXED_STRING_VARIABLE_: { do_indexedStringVariable ();
} break; case VARIABLE_REFERENCE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushVariable (var);
} break; case SELF0_: { do_self0 (row, col);
} break; case SELFSTR0_: { do_selfStr0 (row, col);
} break; case OBJECT_: { pushObject (f [programPointer]. content.object);
} break; case TO_OBJECT_: { do_toObject ();
} break; case SELFMATRIX1_: { do_selfMatrix1 (row);
} break; case SELFMATRIX1_STR_: { do_selfMatrix1_STR (row);
} break; case SELFMATRIX2_: { do_selfMatrix2 ();
} break; case SELFMATRIX2_STR_: { do_selfMatrix2_STR ();
} break; case SELFFUNCTION1_: { do_selfFunction1 (row);
} break; case SELFFUNCTION2_: { do_selfFunction2 ();
} break; case OBJECTCELL0_: { do_objectCell0 (row, col);
} break; case OBJECTCELL1_: { do_objectCell1 (row);
} break; case OBJECTCELL1_STR_: { do_objectCell1_STR (row);
} break; case OBJECTCELL2_: { do_objectCell2 ();
} break; case OBJECTCELL2_STR_: { do_objectCell2_STR ();
} break; case OBJECTLOCATION0_: { do_objectLocation0 (row, col);
} break; case OBJECTLOCATION1_: { do_objectLocation1 (row);
} break; case OBJECTLOCATION2_: { do_objectLocation2 ();
} break; case MATRIX0_: { do_matrix0 (row, col);
} break; case MATRIX1_: { do_matrix1 (row);
} break; case MATRIX1_STR_: { do_matrix1_STR (row);
} break; case MATRIX2_: { do_matrix2 ();
} break; case MATRIX2_STR_: { do_matrix2_STR ();
} break; case FUNCTION0_: { do_function0 (row, col);
} break; case FUNCTION1_: { do_function1 (row);
} break; case FUNCTION2_: { do_function2 ();
} break; case ROW_STR_: { do_row_STR ();
} break; case COL_STR_: { do_col_STR ();
} break; case SQR_: { do_sqr ();
} break; case STRING_: {
	autostring32 string = Melder_dup (f [programPointer]. content.string);
	pushString (string.move());
} break; case TENSOR_LITERAL_: { do_tensorLiteral ();
} break; case TENSOR_LITERAL_CELL_: { do_tensorLiteralCell ();
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumber (var -> numericValue);
} break; case NUMERIC_VECTOR_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumericVectorReference (var -> numericVectorValue.get());
} break; case NUMERIC_MATRIX_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumericMatrixReference (var -> numericMatrixValue.get());
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	autostring32 string = Melder_dup (var -> stringValue.get());
	pushString (string.move());
} break; case STRING_ARRAY_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushStringVectorReference (var -> stringArrayValue.get());
} break; default: Melder_throw (U"Symbol “", Formula_instructionNames [parse [programPointer]. symbol], U"” without action.");
			} // endswitch
			programPointer ++;
		} // endwhile
		if (stackPointer != 1)
			Melder_crash (U"Formula: stackpointer ends at ", stackPointer, U" instead of 1.");
		/*
			Move the result from the stack to `result`.
		*/
		result -> reset();
		if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC) {
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_STRING_ARRAY)
				Melder_throw (U"Found a string vector expression instead of a numeric expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMBER);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
			result -> numericResult = theStack [1]. number;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_STRING) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression (value ", theStack [1]. number, U") instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a string expression.");
			if (theStack [1]. which == Stackel_STRING_ARRAY)
				Melder_throw (U"Found a string vector expression instead of a string expression.");
			Melder_assert (theStack [1]. which == Stackel_STRING);
			result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
			Melder_assert (! result -> stringResult);
			result -> stringResult = theStack [1]. moveString();
			Melder_assert (theStack [1]. which == Stackel_STRING);
			Melder_assert (! theStack [1]. getString());
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_STRING_ARRAY)
				Melder_throw (U"Found a string vector expression instead of a numeric vector expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMERIC_VECTOR);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
			result -> numericVectorResult = theStack [1]. numericVector;
			result -> owned = theStack [1]. owned;
			theStack [1]. owned = false;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_STRING_ARRAY)
				Melder_throw (U"Found a string vector expression instead of a numeric matrix expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMERIC_MATRIX);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
			result -> numericMatrixResult = theStack [1]. numericMatrix;
			result -> owned = theStack [1]. owned;
			theStack [1]. owned = false;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_STRING_ARRAY) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression instead of a string vector expression.");
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a string vector expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a string vector expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a string vector expression.");
			Melder_assert (theStack [1]. which == Stackel_STRING_ARRAY);
			result -> expressionType = kFormula_EXPRESSION_TYPE_STRING_ARRAY;
			result -> stringArrayResult = theStack [1]. stringArray;
			result -> owned = theStack [1]. owned;
			theStack [1]. owned = false;
		} else {
			Melder_assert (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_UNKNOWN);
			if (theStack [1]. which == Stackel_NUMBER) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
				result -> numericResult = theStack [1]. number;
			} else if (theStack [1]. which == Stackel_STRING) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
				Melder_assert (! result -> stringResult);
				result -> stringResult = theStack [1]. moveString();
				Melder_assert (theStack [1]. which == Stackel_STRING);
				Melder_assert (! theStack [1]. getString());
			} else if (theStack [1]. which == Stackel_NUMERIC_VECTOR) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
				result -> numericVectorResult = theStack [1]. numericVector;
				result -> owned = theStack [1]. owned;
				theStack [1]. owned = false;
			} else if (theStack [1]. which == Stackel_NUMERIC_MATRIX) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
				result -> numericMatrixResult = theStack [1]. numericMatrix;
				result -> owned = theStack [1]. owned;
				theStack [1]. owned = false;
			} else if (theStack [1]. which == Stackel_STRING_ARRAY) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_STRING_ARRAY;
				result -> stringArrayResult = theStack [1]. stringArray;
				result -> owned = theStack [1]. owned;
				theStack [1]. owned = false;
			} else {
				Melder_throw (U"Don't know yet how to write ", theStack [1]. whichText(), U".");
			}
		}
		/*
			Clean up the stack (theStack [1] has probably been disowned).
		*/
		for (stackPointer = stackPointerMax; stackPointer > 0; stackPointer --)
			theStack [stackPointer]. reset();
	} catch (MelderError) {
		/*
			Clean up the stack (theStack [1] has probably not been disowned).
		*/
		for (stackPointer = stackPointerMax; stackPointer > 0; stackPointer --)
			theStack [stackPointer]. reset();
		if (Melder_hasError (U"Script exited.")) {
			throw;
		} else {
			Melder_throw (U"Formula not run.");
		}
	}
}

/* End of file Formula.cpp */
