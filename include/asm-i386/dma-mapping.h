#ifndef _ASM_I386_DMA_MAPPING_H
#define _ASM_I386_DMA_MAPPING_H

#include <linux/mm.h>

#include <asm/cache.h>
#include <asm/io.h>
#include <asm/scatterlist.h>

#define dma_alloc_noncoherent(d, s, h, f) dma_alloc_coherent(d, s, h, f)
#define dma_free_noncoherent(d, s, v, h) dma_free_coherent(d, s, v, h)

void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, int flag);

void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle);


/**
 * ��ʽDMAӳ����ÿ�����ݴ���ǰ����ӳ�䣬��ʱ������������Ҫ�������÷�������̬�����ڴ滺������
 * dma_map_single�������ǽ�����ʽDMAӳ�䡣�����ջ����������Ե�ַ��������Ӧ�����ߵ�ַ��
 */
static inline dma_addr_t
dma_map_single(struct device *dev, void *ptr, size_t size,
	       enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
	flush_write_buffers();
	return virt_to_phys(ptr);
}

/**
 * �ͷ���ʽDMAӳ��
 */
static inline void
dma_unmap_single(struct device *dev, dma_addr_t dma_addr, size_t size,
		 enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}

/**
 * ӳ��һ����ɢ���ۼ�DMA��
 *		nents:		����ķ�ɢ����ڵ�������
 * ����ֵ��Ҫ���͵�DMA��������������С��nents��
 */
static inline int
dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	   enum dma_data_direction direction)
{
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nents; i++ ) {
		BUG_ON(!sg[i].page);

		sg[i].dma_address = page_to_phys(sg[i].page) + sg[i].offset;
	}

	flush_write_buffers();
	return nents;
}

/**
 * ��Ҫ���߶��ڴ�������DMAʱ������ʹ��pci_map_page����dma_map_page
 */
static inline dma_addr_t
dma_map_page(struct device *dev, struct page *page, unsigned long offset,
	     size_t size, enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
	return page_to_phys(page) + offset;
}

/**
 * �ͷŸ߶��ڴ滺����
 */
static inline void
dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
	       enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}

/**
 * �����ɢ���ۼ�IOӳ�䡣
 */
static inline void
dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
	     enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}

static inline void
dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle, size_t size,
			enum dma_data_direction direction)
{
}

static inline void
dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle, size_t size,
			enum dma_data_direction direction)
{
	flush_write_buffers();
}

static inline void
dma_sync_single_range_for_cpu(struct device *dev, dma_addr_t dma_handle,
			      unsigned long offset, size_t size,
			      enum dma_data_direction direction)
{
}

static inline void
dma_sync_single_range_for_device(struct device *dev, dma_addr_t dma_handle,
				 unsigned long offset, size_t size,
				 enum dma_data_direction direction)
{
	flush_write_buffers();
}

/**
 * �ڶ�������ǰ����ΪDMAд�ڴ�ʱ��Ӳ�����ٻ��治���֪���ڴ����ݵı仯
 * ���ԣ���Ҫ���ô˺���ʹ����Ӧ��Ӳ�����ٻ���ʧЧ��
 * ��x86�У��˺���Ϊ�գ���������Ϊx86ά����Ӳ�����ٻ����DMA֮���һ���ԡ�
 * ����ƽ̨��һ���ˡ�
 */
static inline void
dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg, int nelems,
		    enum dma_data_direction direction)
{
}

/**
 * ���������ڿ�ʼ��RAM���豸��DMA���ݴ���ǰ��Ӧ�õ���dma_sync_sg_for_device
 * ��ˢ����DMA��������Ӧ�ĸ��ٻ����С�ȷ��д���ڴ�����ݣ�ȷʵд����ʵ���ڴ����ˡ�
 */
static inline void
dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg, int nelems,
		    enum dma_data_direction direction)
{
	flush_write_buffers();
}

static inline int
dma_mapping_error(dma_addr_t dma_addr)
{
	return 0;
}

static inline int
dma_supported(struct device *dev, u64 mask)
{
        /*
         * we fall back to GFP_DMA when the mask isn't all 1s,
         * so we can't guarantee allocations that must be
         * within a tighter range than GFP_DMA..
         */
        if(mask < 0x00ffffff)
                return 0;

	return 1;
}

/**
 * ��������Ƿ���Խ��ո�����С�����ߵ�ַ��������ԣ���֪ͨ���߲㣺��������Χ�豸��ʹ�øô�С�����ߵ�ַ��
 */
static inline int
dma_set_mask(struct device *dev, u64 mask)
{
	if(!dev->dma_mask || !dma_supported(dev, mask))
		return -EIO;

	*dev->dma_mask = mask;

	return 0;
}

static inline int
dma_get_cache_alignment(void)
{
	/* no easy way to get cache size on all x86, so return the
	 * maximum possible, to be safe */
	return (1 << L1_CACHE_SHIFT_MAX);
}

#define dma_is_consistent(d)	(1)

static inline void
dma_cache_sync(void *vaddr, size_t size,
	       enum dma_data_direction direction)
{
	flush_write_buffers();
}

#define ARCH_HAS_DMA_DECLARE_COHERENT_MEMORY
extern int
dma_declare_coherent_memory(struct device *dev, dma_addr_t bus_addr,
			    dma_addr_t device_addr, size_t size, int flags);

extern void
dma_release_declared_memory(struct device *dev);

extern void *
dma_mark_declared_memory_occupied(struct device *dev,
				  dma_addr_t device_addr, size_t size);

#endif