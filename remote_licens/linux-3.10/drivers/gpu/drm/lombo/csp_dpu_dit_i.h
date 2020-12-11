
/* Interrupt source of dit */
typedef enum tag_dit_irq_src {
	DIT_IRQ_WB_FINISH       = 1 << 0,   /* trig when finished */
	DIT_IRQ_WB_OVERFLOW  = 1 << 1,   /* trig when fifo overflow */
	DIT_IRQ_WB_TIMEOUT    =  1 << 2,   /* trig when wb timeout */
} __dit_irq_src_t;

typedef enum tag_dit_field_pol {
	DIT_FIRST_FIELD = 0,
	DIT_SECOND_FIELD = 1,
} __dit_field_pol_t;

typedef enum tag_dit_field_seq {
	DIT_TFF_SEQ = 0,
	DIT_BFF_SEQ = 1,
} __dit_field_seq_t;
