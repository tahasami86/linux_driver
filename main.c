#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Taha Sami <tahasami8@gmail.com>");
MODULE_DESCRIPTION("Hello, world! driver for Testing");


#define BAR0 0
#define VENDOR_ID 0x1234
#define DEVICE_ID 0x1111

static const struct pci_device_id my_pci_tbl[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};

MODULE_DEVICE_TABLE(pci,my_pci_tbl);

static int pci_init(struct pci_dev *pdev);

/*
*pci_init: Pci initialization routine
*@pdev : Pci device information
*
*return 0 on success
*/
static int pci_init(struct pci_dev *pdev){
    int status;
    void __iomem *bar0;
    struct device *dev = &pdev->dev;

    dev = dev_get_drvdata(&pdev->dev);

    status = pcim_enable_device(pdev);
    if (status < 0){
        pr_err("Failed to enable pci device\n");
        return status;
    }

    pci_set_master(pdev);

    /*Reserve bar0 memory resources*/
    status = pcim_iomap_regions(pdev,BIT(BAR0),KBUILD_MODNAME);
    if (status < 0){
        pr_err("Failed to reserve bar0 memory resources\n");
        return status;
    }

    bar0 = pcim_iomap_table(pdev)[BAR0];
    if(bar0 == NULL){
        pr_err("Failed to map bar0\n");
        return -1;
    }
    pr_info("BAR0 is mapped to %p\n",bar0);

    
    return 0;
}


static int config_space_read(struct pci_dev *pdev,const struct pci_device_id *ent){
    
    u16 vid, did;
    u8 capability_ptr;
    u32 bar0, saved_bar0;
    uint8_t err=0;

    pr_info("%s %d Entering config space read \n", __func__, __LINE__);

    /*Reading VENDOR_ID and DEVICE_ID*/
    if(0 != pci_read_config_word(pdev, 0x0, &vid)){
        pr_err("%s %d Failed to read VENDOR_ID\n", __func__, __LINE__);
        return -1;
    }

    pr_info("%s %d VENDOR_ID: %04x\n", __func__, __LINE__,vid);

    if(0 != pci_read_config_word(pdev,0x2, &did)){
        pr_err("%s %d Failed to read DEVICE_ID\n", __func__, __LINE__);
        return -1;
    }

    pr_info("%s %d DEVICE_ID: %04x\n", __func__, __LINE__,did);
    
    /*Reading capability_ptr*/

    if(0 != pci_read_config_byte(pdev,0x34, &capability_ptr)){
        pr_err("%s %d Failed to read capability_ptr\n", __func__, __LINE__);
        return -1;
    }
    if(capability_ptr)
        pr_info("%s %d capability_ptr: %08x The PCI has capabilities\n", __func__, __LINE__,capability_ptr);
    else
        pr_info("%s %d capability_ptr: %08x The PCI has no capabilities\n", __func__, __LINE__,capability_ptr);

    /*Reading BAR0*/
    if(0 != pci_read_config_dword(pdev, 0x10, &bar0)){
        pr_err("%s %d Failed to read BAR0\n", __func__, __LINE__);
        return -1;
    }

    saved_bar0 = bar0;
    pr_info("%s %d BAR0 After Reading is: %04X \n", __func__, __LINE__,bar0);

    if(0 != pci_write_config_dword(pdev, 0x10, 0xffffffff)){
        pr_err("%s %d Failed to write BAR0\n", __func__, __LINE__);
        return -1;
    }
    /*Reading BAR0*/
    if(0 != pci_read_config_dword(pdev, 0x10, &bar0)){
    pr_err("%s %d Failed to read BAR0\n", __func__, __LINE__);
    return -1;
    }

    pr_info("%s %d After writing to BAR0: %04X\n", __func__, __LINE__,bar0);

    if((bar0 & 0x3) == 1)
        pr_info(" BAR0 is IO space\n");
    else
        pr_info("BAR0 is memory space\n");

    bar0 &= 0xfffffffD;
    bar0 = ~bar0;
    bar0++;

    pr_info("%s %d  BAR0 size is %u bytes:\n", __func__, __LINE__,bar0);

    if(0 != pci_write_config_dword(pdev, 0x10, saved_bar0)){
        pr_err("%s %d Failed to write BAR0\n", __func__, __LINE__);
        return -1;
    }   

    if(err != pci_init(pdev)){
        return err;        
    }

    return 0;
    
}

static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent){

    int err=0;
    printk("%s %d probing device \n", __func__, __LINE__);

    if(err != config_space_read(pdev,ent)){
        return -1;
    }
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
