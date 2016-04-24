//
//  Starcounter Lisp
//
//  Parser.cpp
//  Parses Clojure syntax given UTF-8 input.
//  Needs unit tests badly.
//
//  Created by Joachim Wester on 21/04/16.
//  Copyright © 2016 Starcounter AB.
//

#include "Parser.hpp"

ParseSomething Parsers[128] = {
    NULL,    // 0
    NULL,    // 1
    NULL,    // 2
    NULL,    // 3
    NULL,    // 4
    NULL,    // 5
    NULL,    // 6
    NULL,    // 7
    NULL,    // 8
    NULL,    // 9
    NULL,    // 10
    NULL,    // 11
    NULL,    // 12
    NULL,    // 13
    NULL,    // 14
    NULL,    // 15
    NULL,    // 16
    NULL,    // 17
    NULL,    // 18
    NULL,    // 19
    NULL,    // 20
    NULL,    // 21
    NULL,    // 22
    NULL,    // 23
    NULL,    // 24
    NULL,    // 25
    NULL,    // 26
    NULL,    // 27
    NULL,    // 28
    NULL,    // 29
    NULL,    // 30
    NULL,    // 31
    NULL,    // 32
    &(Parser::ParseSymbol),    // 33
    &(Parser::ParseString),    // 34  "
    &(Parser::ParseSymbol),    // 35
    &(Parser::ParseSymbol),    // 36
    &(Parser::ParseSymbol),    // 37
    &(Parser::ParseSymbol),    // 38
    &(Parser::ParseSymbol),    // 39
    &(Parser::ParseList),    // 40 (
    &(Parser::ParseSymbol),    // 41
    &(Parser::ParseSymbol),    // 42
    &(Parser::ParseSymbol),    // 43
    &(Parser::ParseSymbol),    // 44
    &(Parser::ParseSymbol),    // 45
    &(Parser::ParseSymbol),    // 46
    &(Parser::ParseSymbol),    // 47
    NULL,    // 48 0
    NULL,    // 49 1
    NULL,    // 50 2
    NULL,    // 51 3
    NULL,    // 52 4
    NULL,    // 53 5
    NULL,    // 54 6
    NULL,    // 55 7
    NULL,    // 56 8
    NULL,    // 57 9
    NULL,    // 58 :
    NULL,    // 59 ;
    &(Parser::ParseSymbol),    // 60
    &(Parser::ParseSymbol),    // 61
    &(Parser::ParseSymbol),    // 62
    &(Parser::ParseSymbol),    // 63
    NULL,     // 64 @
    &(Parser::ParseSymbol),    // 65
    &(Parser::ParseSymbol),    // 66
    &(Parser::ParseSymbol),    // 67
    &(Parser::ParseSymbol),    // 68
    &(Parser::ParseSymbol),    // 69
    &(Parser::ParseSymbol),    // 70
    &(Parser::ParseSymbol),    // 71
    &(Parser::ParseSymbol),    // 72
    &(Parser::ParseSymbol),    // 73
    &(Parser::ParseSymbol),    // 74
    &(Parser::ParseSymbol),    // 75
    &(Parser::ParseSymbol),    // 76
    &(Parser::ParseSymbol),    // 77
    &(Parser::ParseSymbol),    // 78
    &(Parser::ParseSymbol),    // 79
    &(Parser::ParseSymbol),    // 80
    &(Parser::ParseSymbol),    // 81
    &(Parser::ParseSymbol),    // 82
    &(Parser::ParseSymbol),    // 83
    &(Parser::ParseSymbol),    // 84
    &(Parser::ParseSymbol),    // 85
    &(Parser::ParseSymbol),    // 86
    &(Parser::ParseSymbol),    // 87
    &(Parser::ParseSymbol),    // 88
    &(Parser::ParseSymbol),    // 89
    &(Parser::ParseSymbol),    // 90 Z
    NULL,    // 91 [
    NULL,    // 92 BACKSLASH
    NULL,    // 93 ]
    NULL,    // 94 ^
    &(Parser::ParseSymbol),    // 95 _
    NULL,    // 96 `
    &(Parser::ParseSymbol),    // 97 a
    &(Parser::ParseSymbol),    // 98
    &(Parser::ParseSymbol),    // 99
    &(Parser::ParseSymbol),    // 100
    &(Parser::ParseSymbol),    // 101
    &(Parser::ParseSymbol),    // 102
    &(Parser::ParseSymbol),    // 103
    &(Parser::ParseSymbol),    // 104
    &(Parser::ParseSymbol),    // 105
    &(Parser::ParseSymbol),    // 106
    &(Parser::ParseSymbol),    // 107
    &(Parser::ParseSymbol),    // 108
    &(Parser::ParseSymbol),    // 109
    &(Parser::ParseSymbol),    // 110
    &(Parser::ParseSymbol),    // 111
    &(Parser::ParseSymbol),    // 112
    &(Parser::ParseSymbol),    // 113
    &(Parser::ParseSymbol),    // 114
    &(Parser::ParseSymbol),    // 115
    &(Parser::ParseSymbol),    // 116
    &(Parser::ParseSymbol),    // 117
    &(Parser::ParseSymbol),    // 118
    &(Parser::ParseSymbol),    // 119
    &(Parser::ParseSymbol),    // 120
    &(Parser::ParseSymbol),    // 121
    &(Parser::ParseSymbol),    // 122 z
    NULL,    // 123 {
    NULL,    // 124 |
    NULL,    // 125 }
    NULL,    // 126
    NULL     // 127
};


Char Chars[256] = {
    {},          // 0   NULL
    {},          // 1
    {},          // 2
    {},          // 3
    {},          // 4
    {},          // 5
    {},          // 6
    {},          // 7
    {},          // 8
    {0,1},       // 9   TAB
    {0,1},       // 10  LF \n
    {0,1},       // 11  VERTICAL TAB
    {0,1},       // 12  FORM FEED
    {0,1},       // 13  CR \r
    {},          // 14
    {},          // 15
    {},          // 16
    {},          // 17
    {},          // 18
    {},          // 19
    {},          // 20
    {},          // 21
    {},          // 22
    {},          // 23
    {},          // 24
    {},          // 25
    {},          // 26
    {},          // 27
    {},          // 28
    {},          // 29
    {},          // 30
    {},          // 31
    {0,1},       // 32 SPACE
    {1},         // 33  !
    {0,0,1,1},   // 34  "
    {0,0,1,0},   // 35  #
    {1},         // 36  $
    {0,0,1,0},   // 37  %
    {1},         // 38  &
    {0,0,1,0},   // 39  '
    {0,0,1,1},   // 40  (
    {0,0,1,1},   // 41  )
    {1},         // 42  *
    {1},         // 43  +
    {0},         // 44  ,
    {1},         // 45  -
    {1},         // 46  .
    {1},         // 47  /
    {1},         // 48  0
    {1},         // 49  1
    {1},         // 50  2
    {1},         // 51  3
    {1},         // 52  4
    {1},         // 53  5
    {1},         // 54  6
    {1},         // 55  7
    {1},         // 56  8
    {1},         // 57  9
    {0,0,1,1},   // 58  :
    {0,0,1,1},   // 59  ;
    {1},         // 60  <
    {1},         // 61  =
    {1},         // 62  >
    {1},         // 63  ?
    {0,0,1,1},   // 64  @
    {1},         // 65  A
    {1},         // 66  B
    {1},         // 67  C
    {1},         // 68  D
    {1},         // 69  E
    {1},         // 70  F
    {1},         // 71  G
    {1},         // 72  H
    {1},         // 73  I
    {1},         // 74  J
    {1},         // 75  K
    {1},         // 76  L
    {1},         // 77  M
    {1},         // 78  N
    {1},         // 79  O
    {1},         // 80  P
    {1},         // 81  Q
    {1},         // 82  R
    {1},         // 83  S
    {1},         // 84  T
    {1},         // 85  U
    {1},         // 86  V
    {1},         // 87  W
    {1},         // 88  X
    {1},         // 89  Y
    {1},         // 90  Z
    {0,0,1,1},   // 91  [
    {0,0,1,1},   // 92  BACKSLASH
    {0,0,1,1},   // 93  ]
    {0,0,1,1},   // 94  ^
    {1},         // 95  _
    {0,0,1,1},   // 96  `
    {1},         // 97  a
    {1},         // 98  b
    {1},         // 99  c
    {1},         // 100 d
    {1},         // 101 e
    {1},         // 102 f
    {1},         // 103 g
    {1},         // 104 h
    {1},         // 105 i
    {1},         // 106 j
    {1},         // 107 k
    {1},         // 108 l
    {1},         // 109 m
    {1},         // 110 n
    {1},         // 111 o
    {1},         // 112 p
    {1},         // 113 q
    {1},         // 114 r
    {1},         // 115 s
    {1},         // 116 t
    {1},         // 117 u
    {1},         // 118 v
    {1},         // 119 w
    {1},         // 120 x
    {1},         // 121 y
    {1},         // 122 z
    {0,0,1,1},   // 123 {
    {1},         // 124 |
    {0,0,1,1},   // 125 }
    {0,0,1,1},   // 126 ~
    {},          // 127 DELETE
    {1},         // 128
    {1},         // 129
    {1},         // 130
    {1},         // 131
    {1},         // 132
    {1},         // 133
    {1},         // 134
    {1},         // 135
    {1},         // 136
    {1},         // 137
    {1},         // 138
    {1},         // 139
    {1},         // 140
    {1},         // 141
    {1},         // 142
    {1},         // 143
    {1},         // 144
    {1},         // 145
    {1},         // 146
    {1},         // 147
    {1},         // 148
    {1},         // 149
    {1},         // 150
    {1},         // 151
    {1},         // 152
    {1},         // 153
    {1},         // 154
    {1},         // 155
    {1},         // 156
    {1},         // 157
    {1},         // 158
    {1},         // 159
    {1},         // 160
    {1},         // 161
    {1},         // 162
    {1},         // 163
    {1},         // 164
    {1},         // 165
    {1},         // 166
    {1},         // 167
    {1},         // 168
    {1},         // 169
    {1},         // 170
    {1},         // 171
    {1},         // 172
    {1},         // 173
    {1},         // 174
    {1},         // 175
    {1},         // 176
    {1},         // 177
    {1},         // 178
    {1},         // 179
    {1},         // 180
    {1},         // 181
    {1},         // 182
    {1},         // 183
    {1},         // 184
    {1},         // 185
    {1},         // 186
    {1},         // 187
    {1},         // 188
    {1},         // 189
    {1},         // 190
    {1},         // 191
    {1},         // 192
    {1},         // 193
    {1},         // 194
    {1},         // 195
    {1},         // 196
    {1},         // 197
    {1},         // 198
    {1},         // 199
    {1},         // 200
    {1},         // 201
    {1},         // 202
    {1},         // 203
    {1},         // 204
    {1},         // 205
    {1},         // 206
    {1},         // 207
    {1},         // 208
    {1},         // 209
    {1},         // 210
    {1},         // 211
    {1},         // 212
    {1},         // 213
    {1},         // 214
    {1},         // 215
    {1},         // 216
    {1},         // 217
    {1},         // 218
    {1},         // 219
    {1},         // 220
    {1},         // 221
    {1},         // 222
    {1},         // 223
    {1},         // 224
    {1},         // 225
    {1},         // 226
    {1},         // 227
    {1},         // 228
    {1},         // 229
    {1},         // 230
    {1},         // 231
    {1},         // 232
    {1},         // 233
    {1},         // 234
    {1},         // 235
    {1},         // 236
    {1},         // 237
    {1},         // 238
    {1},         // 239
    {1},         // 240
    {1},         // 241
    {1},         // 242
    {1},         // 243
    {1},         // 244
    {1},         // 245
    {1},         // 246
    {1},         // 247
    {1},         // 248
    {1},         // 249
    {1},         // 250
    {1},         // 251
    {1},         // 252
    {1},         // 253
    {1},         // 254
    {1}          // 255
};







