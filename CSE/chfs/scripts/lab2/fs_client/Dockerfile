FROM cse-lab2-base

# Mountpoint
RUN mkdir -p /tmp/mnt
RUN chmod -R o+w /tmp/mnt

# Set the working directory
WORKDIR /home/stu

# Run the binary
CMD ["./fs_client", "/tmp/mnt"]