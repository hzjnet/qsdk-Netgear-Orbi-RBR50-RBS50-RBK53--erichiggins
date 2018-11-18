#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

/* For tlc59208f begin */
#define I2C_ADDR                0x21  //7 bit 
#define I2C_READ_ADDR           0x41  //8 bit
#define I2C_WRITE_ADDR          0x40  //8 bit

#define LED_MODE1_REG           0x00
#define LED_MODE2_REG           0x01
#define LED_PWM0_REG            0x02
#define LED_PWM1_REG            0x03
#define LED_PWM2_REG            0x04
#define LED_PWM3_REG            0x05
#define LED_PWM4_REG            0x06
#define LED_PWM5_REG            0x07
#define LED_PWM6_REG            0x08
#define LED_PWM7_REG            0x09
#define LED_GRPPWM_REG          0x0A
#define LED_GRPFREQ_REG         0x0B
#define LED_OUT0_REG            0x0C
#define LED_OUT1_REG            0x0D

#define LED_OUT0                0
#define LED_OUT1                1

#define LED_POWER               0x01
#define LED_SEQUENCE_1          0x02
#define LED_SEQUENCE_2          0x03
#define LED_SEQUENCE_3          0x04
#define LED_SEQUENCE_4          0x05
#define LED_SEQUENCE_5          0x06
#define LED_SEQUENCE_6          0x07
#define LED_SEQUENCE_7          0x08
#define LED_SEQUENCE_8          0x09
#define LED_ALL                 0x0A
#define LED_ALL_PULSE           0x0B
#define LED_PWM_STATUS          0x0E
#define LED_FREQ_STATUS         0x0F

#define LED_SEQUENCE_1_EN       0x01
#define LED_SEQUENCE_2_EN       0x04
#define LED_SEQUENCE_3_EN       0x10
#define LED_SEQUENCE_4_EN       0x40
#define LED_SEQUENCE_5_EN       0x01
#define LED_SEQUENCE_6_EN       0x04
#define LED_SEQUENCE_7_EN       0x10
#define LED_SEQUENCE_8_EN       0x40
#define LED_ALL_EN              0x55
/* For tlc59208f end */

static struct i2c_client *g_client = NULL;

int tlc59208f_read(u8 reg)
{
	int ret;
	//printk(" client name=[%s]\n ",g_client->name);
	//printk(" client addr =[%x]\n",(g_client->addr & (~(1<<7))));
	if(g_client == NULL)
	{
		printk(KERN_ERR "tlc59208f i2c g_client dont exit\n");
		return -1;
	}
	ret = i2c_smbus_read_byte_data(g_client,reg);
	if(ret < 0){
		printk(KERN_ERR "tlc59208f i2c read fail! reg=%d \n",reg);
	}
	return ret;
}

int tlc59208f_write(u8 reg, u8 val)
{
	int ret;

	if(g_client == NULL)
	{
		printk(KERN_ERR "tlc59208f i2c g_client dont exit(write)\n");
		return -1;
	}

	ret = i2c_smbus_write_byte_data(g_client,reg,val);
	
	if(ret < 0){
		printk(KERN_ERR "tlc59208f i2c write fail! reg =%d\n",reg);
	}

	return ret;
}


int tlc59208f_reg_init(char mode1, char mode2, char out0, char out1)
{
	tlc59208f_write(LED_MODE1_REG,mode1);
	tlc59208f_write(LED_MODE2_REG,mode2);
    tlc59208f_write(LED_OUT0_REG,out0);
    tlc59208f_write(LED_OUT1_REG,out1);
}
EXPORT_SYMBOL_GPL(tlc59208f_reg_init);

int tlc59208f_config(u8 port_gpio ,u8 port_val)
{
	unsigned int output_val=0;

    switch(port_gpio)
    {
	    case LED_SEQUENCE_1:
            output_val = tlc59208f_read(LED_PWM0_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM0_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_2:
            output_val = tlc59208f_read(LED_PWM1_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM1_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_3:
            output_val = tlc59208f_read(LED_PWM2_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM2_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_4:
            output_val = tlc59208f_read(LED_PWM3_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM3_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_5:
            output_val = tlc59208f_read(LED_PWM4_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM4_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_6:
            output_val = tlc59208f_read(LED_PWM5_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM5_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_7:
            output_val = tlc59208f_read(LED_PWM6_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM6_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_SEQUENCE_8:
            output_val = tlc59208f_read(LED_PWM7_REG);
            //printk("Before write value:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_PWM7_REG,output_val);
            //printk("ready to write value= [%x]\n",output_val);
	      break;
	    case LED_ALL:
	        tlc59208f_write(LED_MODE2_REG,0x02);
            output_val = tlc59208f_read(LED_GRPPWM_REG);
            //printk("Before write group pwm:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_GRPPWM_REG,output_val);
            //printk("ready to write group pwm= [%x]\n",output_val);
	      break;
        case LED_ALL_PULSE:
	        tlc59208f_write(LED_MODE2_REG,0x22);
            output_val = tlc59208f_read(LED_GRPFREQ_REG);
            //printk("Before write group freq:%x\n",output_val);
            output_val = port_val;    
			tlc59208f_write(LED_GRPFREQ_REG,output_val);
            //printk("ready to write group freq= [%x]\n",output_val);
	      break;
	    case LED_PWM_STATUS:
            output_val = tlc59208f_read(LED_GRPPWM_REG);
			return output_val;
	      break;
	    case LED_FREQ_STATUS:
            output_val = tlc59208f_read(LED_GRPFREQ_REG);
			return output_val;
	      break;
        deafult:
		  break;
    }
}
EXPORT_SYMBOL_GPL(tlc59208f_config);

static ssize_t show_I2C_tlc59208f(struct device *dev, struct device_attribute *attr, char *buf)
{

}

static ssize_t set_I2C_tlc59208f(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{

}

static DEVICE_ATTR(externoutput, S_IRUGO | S_IWUSR, show_I2C_tlc59208f, set_I2C_tlc59208f);
static struct attribute *tlc59208f_sysfs_entries[] = {
	&dev_attr_externoutput.attr,
	NULL
};

static struct attribute_group tlc59208f_attr_group = {
	.name = NULL,
	.attrs = tlc59208f_sysfs_entries,
};

static int tlc59208f_suspend(struct i2c_client *client, pm_message_t state)
{
	printk("tlc59208f suspend\n");
	return 0;
}

static int tlc59208f_resume(struct i2c_client *client)
{
	printk("tlc59208f resume\n");
	return 0;
}

static int tlc59208f_shutdown(struct i2c_client *client)
{
        printk("tlc59208f shutdown\n");
        return 0;
}

static int tlc59208f_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret=0;

	printk("tlc59208f probe\n");
	
	g_client = client;
	
	ret = sysfs_create_group(&client->dev.kobj, &tlc59208f_attr_group);
	if(ret) {
		printk("error creating sysfs group\n");
		return -1;
	}
	
	return 0;
}

static int tlc59208f_remove(struct i2c_client *client)
{
	printk("tlc59208f_remove\n!");
	sysfs_remove_group(&client->dev.kobj, &tlc59208f_attr_group);
	return 0;
}

static const struct i2c_device_id tlc59208f_id[] = {
	{"tlc59208f", 0x27},
	{}
};
static struct i2c_driver tlc59208f_driver = {
	.driver = {
	 	.name = "tlc59208f",
	},
	.probe = tlc59208f_probe,
	.remove = tlc59208f_remove,
	.id_table = tlc59208f_id,
	.suspend = tlc59208f_suspend,
	.resume = tlc59208f_resume,
	.shutdown = tlc59208f_shutdown,
};

static int __init tlc59208f_init(void)
{
	printk("tlc59208f init!\n");
	return i2c_add_driver(&tlc59208f_driver);
}

static void __exit tlc59208f_exit(void)
{
	i2c_del_driver(&tlc59208f_driver);
}

module_init(tlc59208f_init);
module_exit(tlc59208f_exit);

MODULE_DESCRIPTION("tlc59208f driver");
MODULE_LICENSE("GPL");

