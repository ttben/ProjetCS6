        TEXT

main    EQU *
        ADD R0, #12345
        CALL NC, 6[R0]
        NOP
        NOP

	HALT
        END

        DATA

        WORD 0
        WORD 0
        WORD 0
        WORD 0

        END
