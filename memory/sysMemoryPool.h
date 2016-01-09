//---------------------------------------------------------------------------
#ifndef sysMemoryPoolH
#define sysMemoryPoolH
//---------------------------------------------------------------------------

/**
this memory pool allocator have std interface and can be using in stl templates
as allocator
*/
//std::allocator
namespace memory {
	template<class T>
	class sysMemoryPool {
		private:
			struct Link {
				Link *next;
			};

			struct Page {
				enum {size = 4080}; //4 Kb minus 16 bytes, for data memory alighment
				char buffer[size];
				Page *next;
			};

			Link *head;
			Page *pages;
		public:
			typedef T value_type;
			typedef size_t size_type;
			DWORD page_size;
			size_t element_size;

			sysMemoryPool() {
				head = 0;
				pages = 0;
				element_size = (sizeof(T) < sizeof(Link *)) ? sizeof(Link *) : sizeof(T);
			}

			// allocate memory for n (count) elements by sizeof(T) each
			// return ptr of allocated memory block
			T* allocate(size_type n, void* = 0) {
				if (n == 1) {
					if (head == 0) {
						newPage();
					}
					Link *p = head;
					head = p->next;

					return (T *)p;
				}

				return 0;
			}

			void deallocate(void *p, size_type n) {
				Link *element = static_cast<Link *>(p);
				element->next = head;
				head = element;
			}

			void newPage() {
				Page *page = new Page;
				page->next = pages;
				pages = page;

				const size_t count = Page::size / element_size;
				char *begin = page->buffer;
				char *end = &(page->buffer[(count - 1) * element_size]);
				for (char *p = begin; p != end; p += element_size) {
					reinterpret_cast<Link *>(p)->next = reinterpret_cast<Link *>(p + element_size);
				}
				reinterpret_cast<Link *>(end)->next = 0;
				head = reinterpret_cast<Link *>(begin);
			}

			~sysMemoryPool() {
				Page *page = pages;
				while(page) {
					Page *next = page->next;
					delete page;
					page = next;
				}
			}
			/*
			template<class U>
			sysMemoryPool(const sysMemoryPool<U>& alloc) : std::allocator(alloc) {
			}
			*/
	};
};

#endif
