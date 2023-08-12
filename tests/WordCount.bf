[
	Source: http://www.hevanet.com/cristofd/brainfuck/
	        Daniel B Cristofani (cristofdathevanetdotcom)

	Counts lines, words, bytes. Assumes no-change-on-EOF or EOF->0.
	Example Usage: cat file.txt | bf WordCount.bf
]

>>>+>>>>>+>>+>>+[<<],[
    -[-[-[-[-[-[-[-[<+>-[>+<-[>-<-[-[-[<++[<++++++>-]<
        [>>[-<]<[>]<-]>>[<+>-[<->[-]]]]]]]]]]]]]]]]
    <[-<<[-]+>]<<[>>>>>>+<<<<<<-]>[>]>>>>>>>+>[
        <+[
            >+++++++++<-[>-<-]++>[<+++++++>-[<->-]+[+>>>>>>]]
            <[>+<-]>[>>>>>++>[-]]+<
        ]>[-<<<<<<]>>>>
    ],
]+<++>>>[[+++++>>>>>>]<+>+[[<++++++++>-]<.<<<<<]>>>>>>>>]

