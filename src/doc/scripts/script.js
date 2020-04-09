document.addEventListener('DOMContentLoaded', (event) => {
    document.querySelectorAll('pre').forEach((block, index) => {
        hljs.highlightBlock(block);
        
        var $this = jQuery(block);
        $this.prop('id', 'code-'+index);
        $this.wrap('<div class="code-container"></div>');
        $this.parent().prepend('<button class="copy-button" data-clipboard-target="#code-'+index+'" title="Copy code"><svg xmlns="http://www.w3.org/2000/svg" width="16.104" height="20" viewBox="0 0 16.104 20"><path d="M17.052,4.766a.735.735,0,0,0-.544-.2H10c-.435,0-1,.261-1,.675V19.021A1.178,1.178,0,0,0,10,20H20.317c.435,0,.653-.566.653-.979V9.1a.668.668,0,0,0-.174-.522ZM19.207,9.14H16.4V6.333Zm-8.466,9.14V6.094h4.135V9.837a.856.856,0,0,0,.827.827h3.743v7.617ZM24.974,3.917,21.187.131A.611.611,0,0,0,20.752,0H14.245c-.37,0-.675.2-.675.544V2.633a.641.641,0,0,0,.653.653.655.655,0,0,0,.653-.653V1.306h4.57V4.984a.678.678,0,0,0,.675.675H23.8v8.052h-.892a.641.641,0,0,0-.653.653.655.655,0,0,0,.653.653H24.56c.37,0,.544-.326.544-.7V4.4A.727.727,0,0,0,24.974,3.917ZM20.752,1.523l21.807,2.829H20.752Z" transform="translate(-9)" fill="#ffffff"/></svg></button>');
    });
    setTimeout(() => {
        var clipboard = new ClipboardJS('.copy-button');

        clipboard.on('success', function(e){
            var $this = jQuery(e.trigger);
            $this.prepend('<span class="tooltip">Copied!</span>');
            setTimeout(() => {
                jQuery('span', e.trigger).remove();
            }, 3 * 1000);
        });
    }, 0);
});