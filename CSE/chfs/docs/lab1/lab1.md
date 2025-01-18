# Lab 1: Basic Filesystem

## Get Ready

### Lab 1 Introduction

In Lab 1, you will implement a single-machine (threaded) inode-based filesystem step by step. Below is the filesystem's architecture:

![overall-arch](./lab1_1.png)

Our filesystem consists of three layers: block layer, inode layer and filesystem layer.

The block layer implements a block device which provides APIs to allocate/deallocate blocks, read/write data from/to the blocks. 

The inode layer manages the blocks provided by the block layer in the form of inode. This layer provides APIs to allocate/deallocate inodes, read/write data from/to these inodes. The superblock is also in this layer, which records some critical information of the filesystem.

The filesystem layer provides some basic filesystem APIs, including file operation APIs and directory operation APIs.

### Get Source Code

- Clone the source code from our GitLab:

```bash
git clone https://ipads.se.sjtu.edu.cn:1312/lab/cse-2024-fall.git chfs -b lab1
```

- Change the permissions of the directory. The following command is used to grant write (`w`) permission to other users (`o`) recursively (`-R`) for all files and directories within the directory `chfs`. This operation is necessary because we will write within this directory inside a docker container.

```bash
chmod -R o+w chfs
```

- Under the `chfs` directory, get the git submodules:

```bash
git submodule init
git submodule update
```

### Docker Container

We use docker container for all of your CSE labs, and we have provided a container image including all the environments your need for these labs. 
This will simplify the configuration of the environments.

If you are not familiar with docker container, [this tutorial](https://www.runoob.com/docker/docker-container-usage.html) may help you quickly grasp how to use docker container. If you haven't install docker before, please install docker on your own workspace. You can refer [this tutorial](https://docs.docker.com/desktop/install/windows-install/) to install docker.

- To get the docker image, you have two choices:
    - Pull from remote repository:

    ```bash
    docker pull registry.cn-shenzhen.aliyuncs.com/cse-lab/cse-lab:v1 

    docker tag registry.cn-shenzhen.aliyuncs.com/cse-lab/cse-lab:v1 chfs_image
   ```
    - Build the docker image locally. Execute the following command inside the directory `chfs`:

    ```bash
    docker build -t chfs_image .
    ```

- Create a docker container. You only need to create the docker container once if you have not deleted the container. Execute the following command inside the directory `chfs`. The following command mounts the `chfs` directory to the docker container, which means that all the changes to the files inside the directory will be reflected inside the docker container, and vice versa.

```bash
docker create -t -i --privileged --name chfs -v $(pwd):/home/stu/chfs chfs_image bash
```

- Start the docker container:

```bash
docker start -a -i chfs
```

- Now you are inside the docker container, and it should look like this:

```bash
stu@xxxxxxxx:~$
```

- The username is `stu` and the password is `000`.

- You can write the code outside the container, but you must compile your code and execute the test scripts inside this container.

- You can type the `exit` command or press `Ctrl+D` inside the container to stop the container, then you will return host shell. Please don't forget to start container next time.

- You can use vscode and install the `Dev Containers` extension to develop in the docker container conveniently.

### Implementation

We break down this lab into three parts according to the three layers of the filesystem you will implement. For each part, you have to implement some functions. While We have written most of the codes, we have left some parts incomplete and marked them with an `UNIMPLEMENTED()`. What you have to do is to delete the `UNIMPLEMENTED()` tag and fill in your implementation.

### Compile Code

Inside the container, enter the directory `chfs`, and execute the following commands:

```bash
mkdir build
cd build
cmake ..
make build-tests -j
make fs -j
```

### Test

We have prepared two kinds of tests for Lab 1: the unit tests and the integration tests.

Each unit test checks the correctness of a function you have implemented. After you have finished the implementation of a part, you have to check whether your implementation can pass specific unit tests to ensure the correctness of your code.

To run the unit tests, execute the following commands under `build` directory:

```bash
make build-tests -j
make test -j
```

If you configured the environment correctly, you may find that you will pass the `BasicTest`. This is because we have implemented a basic bitmap for you.
If you can't pass any test, please check your environment.

For the integration tests, we will mount the filesystem you have implemented and execute some real filesystem operations such as `ls`, `echo`, etc to test the correctness of your implementation.

To run the integration tests, first compile the adaptor layer (which will be introduced in later parts). Execute the following command under `build` directory:

```bash
make fs -j
```


Then execute the following command under `scripts/lab1` directory:

```bash
./integration_test.sh
```

---

## Demo

Lab 1 implements a simple single-machine inode-based filesystem which can support some basic filesystem operations, such as the creation of a file/directory, the deletion of a file, read/write a file and list the contents of a directory. You can skip this demo part now and start your lab from the next part `Part1: Block Layer`.After finish lab1, you can try to use the filesystem implemented by yourself! Follow the steps:

- Under `chfs` directory, execute:

  ```bash
  ./scripts/lab1/start_fs.sh 
  ```

- After the script returns, there will be a `mnt` directory inside `chfs` directory.

- Enter the `mnt` directory. The filesystem you have implemented is mounted to this directory, which means that every filesystem request inside this directory will be fulfilled by the filesystem you have implemented.

- You can create a new directory:

  ```bash
  mkdir my_dir 
  ```

- Then you can create a new file inside this directory:

  ```bash
  touch my_dir/a.txt 
  ```

- Check whether you have successfully create the file:

  ```bash
  ls my_dir 
  ```

- After that you can write something into this file:

  ```bash
  echo "foo" >> my_dir/a.txt
  echo "bar" >> my_dir/a.txt
  ```

- Read the file, then you will see the contents you have just written:

  ```bash
  cat my_dir/a.txt 
  ```

- Then you do not need this file any more:

  ```bash
  rm my_dir/a.txt 
  ```

- Also, you do not need this directory any more:

  ```bash
  rm -rf my_dir 
  ```

- Then you will see the output:

  ```bash
  rm: cannot remove 'a': Software caused connection abort 
  ```

  This is because this filesystem currently does not support removing a directory. You can refer to the `chfs_rmdir` inside `daemons/single_node_fs/main.cc` and implement it if you are interested.



## Part 1: Block Layer

The block layer implements a block device which provides APIs to allocate/deallocate blocks, read/write data from/to the blocks. 

### Part 1A: Block Manager

In Part 1A, you will implement the block manager of block layer. You have to implement the following functions inside `src/block/manager.cc` (You can only modify this file, do not modify any other files) :
- `write_block`: Write a block to the internal block device. There is no block cache in this lab.
- `write_partial_block`: Write a partial block to the block device, provided the offset and length of the written contents in the block.
- `zero_block`: Clear the contents of a block.
- `read_block`: Read the block contents into a buffer.

You may refer to the definition of class `BlockManager` and the comments of these functions in `src/include/block/manager.h` for more detailed information.

If your implementation is correct, you should pass the unit tests:
- `BlockManagerTest.ReadWritePageTest`
- `BlockManagerTest.ZeroTest`
- `BlockManagerTest.InMemoryTest`

Do not forget to re-compile your project before your test!

You may refer to `test/block/manager_test.cc` for the detailed implementation of these unit tests to help you debug. You can change the test file to print some logs for you to debug, but we will use the original version when grading.

### Part 1B: Block Allocator

In Part 1B, you will implement the block allocator of block layer. The block allocator in this lab utilizes a bitmap to manage the allocation and deallocation of the blocks. The bitmap is stored in some blocks in the block device. You may refer to the definition of class `BlockAllocator` and the comments of these functions in `src/include/block/allocator.h` for more detailed information. You may also refer to `src/include/common/bitmap.h` for the APIs to manipulate a bitmap. Note that the `bitmap` class should be constructed dynamically when you want to manipulate bit in a `u8` buffer.

You have to implement the following functions inside `src/block/allocator.cc` (You can only modify this file, do not modify any other files) :
- `allocate`: Allocate a block, return its block id.
- `deallocate`: Deallocate a block.

If your implementation is correct, you should pass the unit tests:
- `BlockAllocatorTest.Allocation`

You may refer to `test/block/allocator_test.cc` for the detailed implementation of these tests to help you debug.

## Part 2: Inode Layer

The inode layer manages the blocks provided by the block layer in the form of inode. This layer provides APIs to allocate/deallocate inodes, read/write data from/to these inodes. The superblock is also in this layer, which records some critical information of the filesystem.

### Part 2A: Inode and Inode Manager

In Part 2A, you will implement the Inode Manager of the Inode Layer. 
<font color=red size=4>Important: Note that the block layout of this lab is slightly different from the layout you learned in class!!!</font>
Please check below for more details. 

We have implemented the structure of the Inode. You may refer to `src/include/metadata/inode.h` and `src/metadata/inode.cc` for the definition of class `Inode`. In this lab, the layout of one inode fits exactly in a single block. This will simplify your implementation.

The Inode Manager assumes the following layout on block device:

```
| Super block | Inode Table | Inode allocation bitmap | Block allocation bitmap | Other data blocks |
```

- `Inode Table` here is a mapping from the `inode_id` to `block_id`, 
which is different from the class such that the inode table directly stores the block.
We choose this in-direction design in lab because it will allocate blocks for inode table dynamically, rather than pre-allocate them statically. It will have better block utilization at the cost of one more lookup for the inode.

- `Inode allocation bitmap` is a bitmap to indicate the usage of each Inode. If an inode is occupied, the corresponding bit in the bitmap is set to `1`. When an `Inode` is allocated or deallocated, the data in this area should be changed.

- `Block allocation bitmap` is a bitmap to indicate the usage of each Block. If an block is allocated, the corresponding bit in the bitmap is set to `1`. Note that the `Block allocation bitmap` itself is also stored in the block layout.

- `Other data blocks` contains other blocks you allocated by the `BlockAllocator`. The `Inode` you learned in the class is actually stored in these blocks.

**Note that the Inode Table stores the indirection mapping relationships of `inode_id->block_id`.** The block_id is the block which actually stores the `Inode` (Do not forget one Inode matches one block exactly).
You should implement `get` function in `src/metadata/manager.cc` to read the corresponding `Inode Table` block first, then get the `block_id`. Then you can read the actual `Inode` structure via this `block_id`.

Given the id of an inode, to know its index in the Inode Table (and vice versa), you can use the Macros `RAW_2_LOGIC` and `LOGIC_2_RAW` in `src/metadata/manager.cc`. 


Your task in this part is to implement the following function inside `src/metadata/manager.cc` (You can only modify this file, do not modify any other files) :
- `allocate_inode`: Allocate an inode and initialize it with the specific type. This function takes the block id of the inode, because this function assumes that the block where the inode resides has already been allocated.
- `free_inode`: Free an inode.
- `get`: Get the block id of the block where current inode resides.
- `set_table`: Set the block id of an inode in the Inode Table.

If your implementation is correct, you should pass:
- `InodeManagerTest.InitAndTable`
- `InodeManagerTest.Allocation`

You may refer to `test/metadata/inode_manager_test.cc` for the detailed implementation of these tests to help you debug.

## Part 3: Filesystem Layer

The filesystem layer provides some basic filesystem APIs, including file operation APIs and directory operation APIs.

### Part 3A: create

In Part 3A, you will implement `create` file operation of the filesystem layer. You have to implement the following function inside `src/filesystem/data_op.cc` (You can only modify this file, do not modify any other files):
- `alloc_inode`: Allocate an inode and initialize it with the given type. This function will allocate a block for the created inode.

You may refer to `src/include/filesystem/operations.h` for more information of this function.

If your implementation is correct, you should pass:
- `BasicFileSystemTest.Init`
- `FileSystemTest.CreateAndGetAttr`

You may refer to `test/filesystem/basic_fs_test.cc` and `test/filesystem/create_and_getattr_test.cc` for the detailed implementation of these tests to help you debug.

### Part 3B: read and write

In Part 3B, you will implement the file operations `read` and `write`. You have to implement the following functions inside `src/filesystem/data_op.cc` (You can only modify this file, do not modify any other files):
- `read_file`: Read the contents of the blocks of an inode.
- `write_file`: Write to the blocks of an inode. This function will dynamically allocate/deallocate the blocks inside the inode.

You may refer to `src/include/filesystem/operations.h` for the declarations of these functions.

If your implementation is correct, you should pass:
- `FileSystemTest.WriteLargeFile`
- `FileSystemTest.SetAttr`

You may refer to `test/filesystem/indirect_file_write_test.cc` for the detailed implementation of these tests to help you debug.

### Part 3C: operations on directory entries

In this part, you will implement the APIs which manipulates the entries of a directory.

You have to implement the following functions inside `src/filesystem/directory_op.cc` (You can only modify this file, do not modify any other files):
- `parse_directory`: Parse the contents of a directory into entries and store them in a list.
- `read_directory`: Given the inode id of a directory, read its contents parse into entries.
- `append_to_directory`: Given the contents of a directory, append a new `filename->inode_id` entry to the contents.
- `rm_from_directory`: Given the contents of a directory, remove an entry by the given filename from the contents.

You may refer to `src/include/filesystem/directory_op.h` for the directory content storage structure.

If your implementation is correct, you should pass:
- `FileSystemBase.Utilities`
- `FileSystemBase.UtilitiesRemove`
- `FileSystemTest.DirectOperationAdd`

You may refer to `test/filesystem/directory_op_test.cc` for the detailed implementation of these tests to help you debug.

### Part 3D: combine directory and file together

In this part, you have to implement the following functions in `src/filesystem/directory_op.cc` (You can only modify this file, do not modify any other files) which operates on files inside directory:
- `lookup`: Given the filename and the inode id of its parent directory, return its inode id.
- `mk_helper`: The helper function to create a directory or a file inside its parent directory.
- `unlink`: Give the name of a file/directory, remove it from its parent directory and free its blocks.

You may refer to `src/include/filesystem/operations.h` for the declarations of these functions.

If your implementation is correct, you should pass:
- `FileSystemTest.mkdir`

## Integration Test

### Adaptor Layer

In previous parts, you have implemented the main body of your single-machine inode-based filesystem. But to let the other user applications **truely** use the filesystem, there has to be an adaptor layer.

The adaptor layer acts as a translator between the standard filesystem requests made by user applications (such as the `ls`, `echo` commands) and the corresponding operations of your filesystem. It will intercept these requests and relay them to the appropriate functions in your filesystem implementation. This adaptor layer ensures that the filesystem seamlessly integrates with the existing OS infrastructure, allowing applications to interact with files and directories just as they would with any other filesystems. You may refer to the following figure to better understand the role of the adaptor layer:

![adaptor-layer](./lab1_2.png)

In this lab, we use [the libfuse userspace library](http://libfuse.github.io/doxygen/index.html) provided by FUSE (Filesystem in Userspace) to implement the adaptor layer. You can refer to `daemons/single_node_fs/main.cc` for the detailed implementation.

### Run test

To run the integration tests, first you should compile the adaptor layer with the filesystem you have implemented. Execute the following command under `build` directory:

```bash
make fs -j
```

Then execute the following command under `scripts/lab1` directory:

```bash
./integration_test.sh
```

If you pass all the integration tests, you should see the output:

```bash
Passed 5/5 tests
```

You can also execute the following command to execute one specific integration test:

```bash
./integration_test.sh [A|B|C|D|E]
```

### Debug tips

- Carefully read the test scripts (the `test-lab1-part2-*` scripts inside `scripts/lab1`) of these integration tests, some of which are written in Perl (the `.pl` files inside `scripts/lab1` directory). To understand what these tests are doing, [this tutorial](https://www.runoob.com/perl/perl-tutorial.html) may help you quickly grasp Perl.

- Carefully read our implementaion of the adaptor layer in `daemons/single_node_fs/main.cc`. Here are the functions which you should focus on:

    - `chfs_open`
    - `chfs_getattr`
    - `chfs_readdir`
    - `chfs_read`
    - `chfs_mknod`
    - `chfs_mkdir`
    - `chfs_unlink`
    - `chfs_write`
    - `chfs_setattr`
    - `chfs_lookup`

    Each of these functions implements one standard filesystem operation used by other user applications. You can use `std::cout` inside these functions to output more information to help you debug. You may also refer [the the libfuse document](http://libfuse.github.io/doxygen/index.html) to understand the libfuse APIs (the `fuse_xxx` functions) used in the adaptor layer. These functions may help you to understand the behavior of functions you implemented in `src/filesystem/data_op.cc` and `src/filesystem/directory_op.cc`.

- Please note that passing all the unit tests does not guarantee the complete correctness of your implementations. So you may need to go back to fix the bugs  if you cannot pass the integration tests.

## Grading

After you have finished all parts, firstly, compile your code by executing the following commands under `build` directory:

```bash
make build-tests -j
make fs -j
```

Then, for unit tests, execute the following command under `build` directory:

```bash
make test -j
```

For the integration tests, execute the following command under `scripts/lab1` directory:

```bash
./integration_test.sh
```

## Handin

Execute the following command under `scripts/lab1` directory:

```bash
./handin.sh
```

Then you will see a `handin.tgz` file under the root directory of this project. Please rename it in the format of: `lab1_[your student id].tgz`, and upload this `.tgz` file to Canvas.
If you use docker environment, remember to execute the `handin.sh` in the docker environment. You can use `docker cp` command to copy your `handin.tgz` file from the docker environment to host.