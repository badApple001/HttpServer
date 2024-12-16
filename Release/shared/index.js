document.addEventListener("DOMContentLoaded", function () {
    const fileListElement = document.getElementById('file-list');

    // 模拟文件数据，包含文件名和大小（单位：KB）
    const fallbackFiles = [
        { name: 'file1.exe', size: 1234 },
        { name: 'file2.apk', size: 5678 },
        { name: 'file3.exe', size: 910 },
        { name: 'file4.apk', size: 1122 }
    ];

    // 尝试从服务器获取文件列表
    fetch('/api/files')
        .then(response => response.json())
        .then(data => {
            const files = data.files || fallbackFiles;
            updateFileList(files);
        })
        .catch(error => {
            console.error('Error fetching file list:', error);
            updateFileList(fallbackFiles);
        });

    // 更新文件列表的 DOM
    function updateFileList(files) {
        fileListElement.innerHTML = ''; // 清空现有列表
        files.forEach(file => {
            const li = document.createElement('li');

            // 图标
            const icon = document.createElement('span');
            console.log(file.name)
            icon.textContent = getIconForFile(file.name);
            icon.className = 'icon';

            // 文件链接
            const link = document.createElement('a');
            link.href = `/download?file=${encodeURIComponent(file.name)}`;
            link.textContent = file.name;
            link.download = file.name;


            // 文件大小
            const size = document.createElement('span');
            size.className = 'file-size';
            size.textContent = `${(file.size / 1024).toFixed(2)} MB`;

            li.appendChild(icon);
            li.appendChild(link);
            li.appendChild(size);
            fileListElement.appendChild(li);
        });
    }

    // 获取文件类型对应的图标
    function getIconForFile(fileName) {
        if (fileName.endsWith('.exe')) return '🖥️';
        if (fileName.endsWith('.apk')) return '📱';
        return '📄';
    }



    // 使用 window.tsParticles 来初始化
    tsParticles.load("tsparticles", {
        particles: {
            number: {
                value: 150,
                density: { enable: true, value_area: 800 }
            },
            shape: { type: "triangle" },
            color: { value: "#4a90e2" },
            opacity: {
                value: 0.5,
                random: true,
                anim: { enable: true, speed: 1, opacity_min: 0.1, sync: false }
            },
            size: {
                value: 6,
                random: true,
                anim: { enable: true, speed: 4, size_min: 0.1, sync: false }
            },
            move: {
                enable: true,
                speed: 3,
                direction: "none",
                random: false,
                straight: false,
                out_mode: "out",
                bounce: false
            }
        },
        interactivity: {
            detect_on: "canvas",
            events: {
                onhover: { enable: true, mode: "repulse" },
                onclick: { enable: true, mode: "push" }
            },
            modes: {
                repulse: { distance: 100, duration: 0.4 },
                push: { particles_nb: 4 }
            }
        },
        retina_detect: true
    });

});
