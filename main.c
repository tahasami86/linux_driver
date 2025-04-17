#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Taha Sami <tahasami8@gmail.com>");
MODULE_DESCRIPTION("Hello, world! driver for Testing");

static int  __init my_init(void)
{
    printk("Hello, world! driver\n");
    return 0;
}

static void __exit my_exit(void)
{
    printk("Goodbye, world! driver\n");
}

module_init(my_init);
module_exit(my_exit);
