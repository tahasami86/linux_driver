#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Taha Sami <tahasami8@gmail.com>");
MODULE_DESCRIPTION("Hello, world! driver for Testing");

#define VENDOR_ID 0x8086
#define DEVICE_ID 0x29c0

static const struct pci_device_id my_pci_tbl[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};

MODULE_DEVICE_TABLE(pci,my_pci_tbl);


static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent){

    printk("%s %d probing device %04x:%04x\n", __func__, __LINE__,pdev->vendor, pdev->device);
    return 0;
}

static void my_pci_remove(struct pci_dev *pdev){

    printk("%s %d removing device %04x:%04x\n", __func__, __LINE__,pdev->vendor, pdev->device);
    
}
/*PCI Driver struct*/
static struct pci_driver my_pci_driver = {
    .name       ="my_pci_driver",
    .id_table   =my_pci_tbl,
    .probe      =my_pci_probe, 
    .remove     =my_pci_remove
};

static int  __init my_init(void)
{
    printk("Registering, driver\n");
    return pci_register_driver(&my_pci_driver);
}

static void __exit my_exit(void)
{
    printk("Removing, driver\n");
    pci_unregister_driver(&my_pci_driver);
}

module_init(my_init);
module_exit(my_exit);
